// The session replay's client critters: the Levels.txt cmon units (chickens, rats, ...) the client creates on its own when it first
// sees a room, and their client-side AI. The server knows nothing of them; their only visible output is the sounds they play.
//   0x18370 (room first seen, from 0x440C0): per cmon entry, a percent roll on the client room seed, a count roll, 0x181A0 per unit.
//   0x181A0: up to 10 tries of a random spot in the room (x roll, then y roll) free for the class's size under mask 0x3F11; the spot
//            found is created through 0xA7780 (room seed roll -> unit init seed; 0x4D160: start-frame roll, direction roll).
//   0xA8030 per client unit: 0x2B1E0 (animation/path step) then, type 1, 0x17F70 (the critter AI: delay countdown, the nearest
//            player 0x16D80, dormant beyond 30, class 0x95/0x10C -> 0x17E80: the nearest unit 0x16C20/0x16E50, flee 0x17500 when
//            closer than 4, else a 30% random walk 0x173E0 or a five-frame pause).
//   0x4DC70 action 0xC: mode 8 at once (0x2A6C0 -> #11090 -> 0x24490: the mode sound); action 7 (0x17270) at its own spot:
//            0x4DC30 puts a unit in modes 1..15 (not 12) back to 1; action 1: mode 2 when the walk path is made.
// Ours: the client room seeds of client_unit_seed.cpp; the server rooms stand for the client's (same coordinates, collision, the
// server's units for the client's copies of them).
// ponytail: the client path (0x2A040) is not walked: a critter keeps its spot; a walk is taken as made when its end spot is free and
// lasts until the animation's loop ends; mode 8 lasts one animation loop.
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include "../d2moo/include/D2Dungeon.h"
#include "../d2moo/include/D2Collision.h"
#include "../d2moo/include/D2DataTbls.h"
#include "../d2moo/include/DataTbls/LevelsTbls.h"
#include "../d2moo/include/DataTbls/MonsterTbls.h"
#include "../d2moo/include/DataTbls/AnimTbls.h"
#include "../d2moo/include/Units/Units.h"
#include "sound_unit_refs.h"
#include <emscripten.h>

namespace d2client_unitseed { uint32_t roll(const D2ActiveRoomStrc* r); bool known(const D2ActiveRoomStrc* r); }
extern "C" uint32_t d2_client_unit_mode_sounds(uint32_t type, uint32_t id, int32_t cls, uint32_t typeFlags, int32_t superUnique,
                                               uint32_t mode, uint32_t hit, uint32_t cvt, uint32_t flags, int32_t roll);
extern "C" uint32_t d2_client_unit_mode_sound_at(uint32_t i);

namespace {
struct Critter {
    uint32_t id, cls; D2ActiveRoomStrc* room; int32_t x, y, size;
    uint32_t lo, hi;              // unit seed (+0x20/+0x24)
    uint32_t mode; int32_t delay; // +0x10, data +0x30
    uint32_t f44, f48, f4c, u7c;  // animation clock, length, rate; last 0x22CA0 sound frame
    uint32_t born;
};
std::vector<Critter> g_c;
uint32_t g_counter;               // D2Client+0xDE834
std::string g_log;
uint32_t lcg(uint32_t& lo, uint32_t& hi) { const uint64_t v = uint64_t(lo) * 0x6ac690c5ull + hi; lo = uint32_t(v); hi = uint32_t(v >> 32); return lo; }
uint32_t lim(uint32_t lo, uint32_t n) { return (n & (n - 1)) ? lo % n : lo & (n - 1); }
const D2MonStatsTxt* monStats(int32_t cls) { return cls >= 0 && cls < sgptDataTables->nMonStatsTxtRecordCount ? &sgptDataTables->pMonStatsTxt[cls] : nullptr; }
const D2MonStats2Txt* monStats2(const D2MonStatsTxt* m) {
    const int32_t i = m ? (int16_t)m->wMonStatsEx : -1;
    return i >= 0 && i < sgptDataTables->nMonStats2TxtRecordCount ? &sgptDataTables->pMonStats2Txt[i] : nullptr;
}
void anim(Critter& c) {
    const D2AnimDataRecordStrc* a = DATATBLS_GetAnimDataRecord(nullptr, (int)c.cls, (int)c.mode, UNIT_MONSTER, nullptr);
    c.f48 = a ? a->dwFrames << 8 : 0; c.f4c = a ? (uint32_t)a->dwAnimSpeed : 0;
}
// #10522 (unit 1 = the critter)
int32_t distance(const Critter& c, D2UnitStrc* u) {
    D2CoordStrc p{}; UNITS_GetCoords(u, &p);
    int32_t dx = p.nX - c.x, dy = p.nY - c.y; if (dx < 0) dx = -dx; if (dy < 0) dy = -dy;
    const int32_t s = c.size / -2 - UNITS_GetUnitSizeX(u) / 2;
    int32_t nx = s + dx, ny = s + dy; if (nx < 0) nx = 0; if (ny < 0) ny = 0;
    return nx <= ny ? (nx + 2 * ny) / 2 : (ny + 2 * nx) / 2;
}
// 0x2A6C0 -> #11090: a new mode resets the clock and reaches 0x24490
void setMode(Critter& c, uint32_t m, uint32_t frame) {
    if (c.mode == m) return;
    c.mode = m; c.f44 = 0; anim(c);
    char b[96]; snprintf(b, sizeof b, "%u C id=%x mode=%u\n", frame, c.id, m); g_log += b;
    const uint32_t n = d2_client_unit_mode_sounds(1, c.id, (int32_t)c.cls, 0, -1, m, 0, 0, 0, -1);
    for (uint32_t i = 0; i < n; ++i) {
        const int32_t row = (int32_t)(d2_client_unit_mode_sound_at(i) & 0xFFFF);
        // 0x22CA0: the 0xA84 skill row waits 0x19 frames after the unit's last (+0x7C); the port reads +0x7C off a server unit
        if (row == 0xA84) { if (frame - c.u7c < 0x19) continue; c.u7c = frame; }
        d2client_sound_unit_submit(row);
    }
}
// 0x4DC30
void neutral(Critter& c, uint32_t frame) { if (c.mode < 0x10 && c.mode != 0 && c.mode != 0xC) setMode(c, 1, frame); }
// 0x4DC70 through the six-word walk block {x, y, ...}: action 0xC (mode 8 first, then the path), 1 (mode 2 when a path is made)
void walk(Critter& c, int32_t x, int32_t y, uint32_t action, uint32_t frame) {
    char b[96]; snprintf(b, sizeof b, "%u W id=%x a=%u to=%d,%d\n", frame, c.id, action, x, y); g_log += b;
    if (action == 0xC) { setMode(c, 8, frame); return; }
    D2ActiveRoomStrc* r = c.room;
    if (r && !COLLISION_CheckMaskWithSize(r, x, y, c.size, 0x3C01)) setMode(c, 2, frame); else neutral(c, frame);
}
// 0x173E0
void wander(Critter& c, uint32_t step, uint32_t action, uint32_t frame) {
    int32_t dx = (int32_t)(step & 0xFF), dy = dx;
    if (lcg(c.lo, c.hi) & 1) dx = -dx;
    if (lcg(c.lo, c.hi) & 1) dy = -dy;
    walk(c, c.x + dx, c.y + dy, action, frame);
}
// 0x17500: away from the unit, when the next spot that way is free for size 1 under the critter's mask
bool flee(Critter& c, D2UnitStrc* from, uint32_t frame) {
    D2CoordStrc p{}; UNITS_GetCoords(from, &p);
    const uint32_t tx = (uint32_t)c.x, ty = (uint32_t)c.y, sx = (uint32_t)p.nX, sy = (uint32_t)p.nY;
    const int32_t dx = sx > tx ? -1 : sx < tx ? 1 : 0, dy = sy > ty ? -1 : sy < ty ? 1 : 0;
    if (!c.room || COLLISION_CheckMaskWithSize(c.room, c.x + dx, c.y + dy, 1, 0xC01)) return false;
    walk(c, c.x + 4 * dx, c.y + 4 * dy, 0xC, frame);
    return true;
}
// 0x17E80
void chicken(Critter& c, uint32_t frame) {
    if (c.mode != 1 || !c.room) return;
    D2UnitStrc* best = nullptr; int32_t bd = 0x7fffffff;
    D2ActiveRoomStrc** rl = nullptr; int32_t nr = 0; DUNGEON_GetAdjacentRoomsListFromRoom(c.room, &rl, &nr);
    for (uint32_t i = 0; i < (uint32_t)nr; ++i)
        for (D2UnitStrc* u = rl[i]->pUnitFirst; u; u = u->pRoomNext) {   // 0x16E50
            if (u->dwUnitType == UNIT_MONSTER) { if (u->dwClassId == 0x95 || u->dwClassId == 0x22C) continue; }
            else if (u->dwUnitType != UNIT_PLAYER) continue;
            const int32_t d = distance(c, u);
            if (d < bd) { best = u; bd = d; }
        }
    if (!best) return;
    char b[96]; snprintf(b, sizeof b, "%u T id=%x tgt=%u:%x d=%d\n", frame, c.id, best->dwUnitType, best->dwUnitId, bd); g_log += b;
    if (bd < 4) { if (!flee(c, best, frame)) wander(c, 4, 0xC, frame); return; }
    if (lcg(c.lo, c.hi) % 100 < 0x1E) { wander(c, 2, 1, frame); return; }
    c.delay = 5;
}
// 0x17F70
void think(Critter& c, D2UnitStrc* player, uint32_t frame) {
    if (c.delay > 0) { --c.delay; return; }
    int32_t dist = 0x7fffffff; D2UnitStrc* tgt = nullptr;
    if (player && player->dwAnimMode != 0 && player->dwAnimMode != 0xC) { dist = distance(c, player); tgt = player; }
    if (!tgt && !player) { c.delay = 0x19; neutral(c, frame); return; }
    if (dist > 30) { c.delay = dist - 15 > 199 ? 200 : dist - 15; neutral(c, frame); return; }
    if (c.cls == 0x95 || c.cls == 0x10C) chicken(c, frame);
    else neutral(c, frame);
}
// 0x2B1E0 (the clock only): a loop's end returns modes 2 and 8 to neutral (0x4DA60)
void step(Critter& c, uint32_t frame) {
    if (!c.f48) return;
    c.f44 += c.f4c;
    if (c.f44 < c.f48) return;
    c.f44 -= c.f48;
    if (c.mode == 2 || c.mode == 8) setMode(c, 1, frame);
}
// 0x181A0 + 0xA7780 + 0x4D160
void create(D2ActiveRoomStrc* room, int32_t cls, uint32_t frame) {
    const D2MonStatsTxt* m = monStats(cls); const D2MonStats2Txt* m2 = monStats2(m);
    if (!m2) return;
    D2DrlgCoordsStrc rc{}; DUNGEON_GetRoomCoordinates(room, &rc);
    const int32_t size = (int8_t)m2->nSizeX, lw = rc.nSubtileWidth - 1, lh = rc.nSubtileHeight - 1;
    int32_t x = 0, y = 0;
    for (int i = 0;; ) {
        x = rc.nSubtileX + (lw > 0 ? (int32_t)lim(d2client_unitseed::roll(room), (uint32_t)lw) : 0);
        y = rc.nSubtileY + (lh > 0 ? (int32_t)lim(d2client_unitseed::roll(room), (uint32_t)lh) : 0);
        if (!COLLISION_CheckMaskWithSize(room, x, y, size, 0x3F11)) break;
        if (++i >= 10) return;
    }
    if (++g_counter == 0xFFFFFFFFu) g_counter = 0;
    Critter c{}; c.id = g_counter; c.cls = (uint32_t)cls; c.room = room; c.x = x; c.y = y; c.size = size; c.mode = 1; c.born = frame;
    D2ActiveRoomStrc* at = DUNGEON_FindRoomBySubtileCoordinates(room->pAct, x, y);
    c.lo = d2client_unitseed::roll(at ? at : room); c.hi = 666;
    const uint32_t init = c.lo;
    anim(c);
    if (c.f48 >= 1) c.f44 = lim(lcg(c.lo, c.hi), c.f48);
    if (!(m->dwMonStatsFlags & (1u << MONSTATSFLAGINDEX_NPC)) && (m2->nModeFlags[0] & (1u << 2))) lcg(c.lo, c.hi);   // the start direction
    char b[160]; snprintf(b, sizeof b, "%u N id=%x cls=%x at=%d,%d size=%d init=%08x f44=%x f48=%x f4c=%x vel=%d\n", frame, c.id, c.cls, x, y, size, init,
                          c.f44, c.f48, c.f4c, m->nVelocity); g_log += b;
    g_c.push_back(c);
}
}

namespace d2client_critter {
// 1.13c D2Client+0x18370
void room_seen(D2ActiveRoomStrc* room, int32_t levelId, uint32_t frame) {
    if (!room) return;
    { char b[96]; snprintf(b, sizeof b, "%u S lvl=%d room=%d,%d known=%d\n", frame, levelId, room->tCoords.nSubtileX, room->tCoords.nSubtileY, (int)d2client_unitseed::known(room)); g_log += b; }
    if (!d2client_unitseed::known(room)) return;
    const D2LevelsTxt* lv = DATATBLS_GetLevelsTxtRecord(levelId);
    if (!lv) return;
    for (int i = 0; i < 4; ++i) {
        const int32_t cls = (int16_t)lv->wCMon[i];
        if (cls < 0) return;
        const uint32_t pr = d2client_unitseed::roll(room);
        { char b[128]; snprintf(b, sizeof b, "%u R lvl=%d room=%d,%d cls=%x pct=%u roll=%u\n", frame, levelId, room->tCoords.nSubtileX, room->tCoords.nSubtileY, cls, (uint32_t)lv->wCPct[i], pr % 100); g_log += b; }
        if (pr % 100 >= (uint32_t)(int32_t)(int16_t)lv->wCPct[i]) continue;
        const D2MonStatsTxt* m = monStats(cls);
        if (!m) continue;
        const int32_t n = (int32_t)m->nMaxGrp - (int32_t)m->nMinGrp;
        int32_t count = (n > 0 ? (int32_t)lim(d2client_unitseed::roll(room), (uint32_t)n) : 0) + m->nMinGrp;
        if (const int16_t k = (int16_t)lv->wCAmt[i]) count *= k;
        for (; count > 0; --count) create(room, cls, frame);
    }
}
// 1.13c D2Client+0xA8030, the critters' part
void tick(D2UnitStrc* player, uint32_t frame) {
    for (Critter& c : g_c) { step(c, frame); think(c, player, frame); }
}
void reset() { g_c.clear(); g_counter = 0; g_log += "RESET\n"; }
}
extern "C" EMSCRIPTEN_KEEPALIVE const char* d2_session_critter_dump() {
    for (const Critter& c : g_c) {
        char b[128]; snprintf(b, sizeof b, "E id=%x mode=%u f44=%x delay=%d at=%d,%d\n", c.id, c.mode, c.f44, c.delay, c.x, c.y); g_log += b;
    }
    return g_log.c_str();
}
