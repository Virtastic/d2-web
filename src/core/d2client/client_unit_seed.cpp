// D2Client client-unit seeds. Every client unit creator (player 1.13c D2Client+0xA7630, monster +0xA788B,
// item +0x833F0/+0x835C0, generic +0xA7400: object, missile, tile) finds the client room at the unit's
// coordinates (+0xA5A60) and, when there is one, rolls that room's seed (room+0x6C/+0x70) once; the roll is
// the unit's init seed (+0x28) and its seed (+0x20) is SEED_InitLowSeed of it. Coordinates (0,0) skip the
// room lookup and the roll. The client room's seed starts as DUNGEON_AllocRoom's (the DRLG gives client and
// server the same low seed per room).
// The local player's own 0x59 comes first in a frame's adds; it rolls only when it already stands in a client
// room (a player created unplaced carries (0,0) and rolls nothing).
// Ours: the client room seeds live beside the server rooms; a server unit created into a room stands for the
// unit-add packet the client would build it from; client missiles roll where we emit them (vine_c50f0.cpp).
// ponytail: units that reach a room only after creation (a dropped item, a unit moving into a room before the
// client sees it) are not counted where the client would count them; a live player is taken as the local
// one (the sessions are single-player).
#include <cstdint>
#include <unordered_map>
#include <vector>
#include "../d2moo/include/D2Dungeon.h"
#include "../d2moo/include/Units/Units.h"
#include "../d2moo/include/Units/Player.h"
#include "../d2moo/include/Path/Path.h"
#include "../d2moo/include/D2DataTbls.h"
#include "../d2moo/include/DataTbls/MonsterTbls.h"
#include "../d2moo/include/D2Game/UNIT/SUnit.h"

namespace {
struct Seed { uint32_t lo, hi; };
std::unordered_map<const D2ActiveRoomStrc*, Seed> g_room;
std::unordered_map<uint64_t, uint32_t> g_unit;
struct Pending { D2GameStrc* game; uint32_t type, id; };
std::vector<Pending> g_pending;
uint64_t key(uint32_t type, uint32_t id) { return uint64_t(type) << 32 | id; }
}

namespace d2client_unitseed {
// DUNGEON_AllocRoom's low seed for a new room (its seed is {low, 666}); a reused room address starts over.
void room(const D2ActiveRoomStrc* r, uint32_t lowSeed) { if (r) g_room[r] = { lowSeed, 666 }; }
// A client unit created in room r: the room seed's next roll, or 0 when the client has no room there.
void flush();
namespace { uint32_t roll_room(const D2ActiveRoomStrc* r) {
    auto it = r ? g_room.find(r) : g_room.end();
    if (it == g_room.end()) return 0;
    const uint64_t s = uint64_t(it->second.lo) * 0x6ac690c5ull + it->second.hi;
    it->second = { uint32_t(s), uint32_t(s >> 32) };
    return uint32_t(s);
} }
uint32_t roll(const D2ActiveRoomStrc* r) { flush(); return roll_room(r); }
namespace {
uint32_t next(Seed& s) { const uint64_t v = uint64_t(s.lo) * 0x6ac690c5ull + s.hi; s = { uint32_t(v), uint32_t(v >> 32) }; return uint32_t(v); }
}
// 1.13c D2Client+0x4D160 (the client monster init, from the 0xAC unit alloc +0xA7780): its seed {init, 666}
// first rolls the start frame (+0x44 = roll % frame count +0x48, no roll when the count is < 1), then, for a
// monster without the NPC flag (MonStats +0x0D bit 0), rolls again and takes the low six bits as the start
// direction when MonStats2 enables WL (mode flags +0xF0, bit 2; else 0). Base id 0x60 faces 7; hcIdx 0x15F 24,
// 0x161 40, 0x158/0x160/0x165 0. #10706 (D2Common+0x34BE0, PATH_SetDirection) sets it. ponytail: base id
// 0x12D's start direction (+0x16C20) is not modelled; it keeps the server's.
void monster_direction(D2UnitStrc* u, uint32_t init) {
    const D2MonStatsTxt* ms = DATATBLS_GetMonStatsTxtRecord(u->dwClassId);
    if (!ms || !u->pDynamicPath || (ms->dwMonStatsFlags & (1u << MONSTATSFLAGINDEX_NPC))) return;
    Seed s = { init, 666 };
    if (u->dwFrameCountPrecise >= 1) next(s);
    uint8_t dir = 0;
    const D2MonStats2Txt* ms2 = UNITS_GetMonStats2TxtRecordFromMonsterId(u->dwClassId);
    if (ms2 && (ms2->nModeFlags[0] & (1u << 2))) dir = uint8_t(next(s) & 0x3f);
    if (ms->nBaseId == 0x12d) return;
    if (ms->nBaseId == 0x60) dir = 7;
    switch (u->dwClassId) {
    case 0x15f: dir = 0x18; break;
    case 0x161: dir = 0x28; break;
    case 0x158: case 0x160: case 0x165: dir = 0; break;
    default: break;
    }
    PATH_SetDirection(u->pDynamicPath, dir);
}
// A server unit created into a room: the client creates it from its unit-add packet. Missiles are the
// client's own. The adds of one server frame reach the client in send order (a retail recording): the local
// player's own 0x59 first, then the monsters' 0xAC newest-first, then the other units newest-first (the room's
// unit list is LIFO); so a frame's creations are queued and rolled in that order when the frame ends.
void created(D2UnitStrc* u) {
    if (!u || u->dwUnitType == UNIT_MISSILE || !u->pGame) return;
    g_pending.push_back({ u->pGame, u->dwUnitType, u->dwUnitId });
}
void flush() {
    if (g_pending.empty()) return;
    std::vector<Pending> q;
    q.swap(g_pending);
    auto rank = [](uint32_t t) { return t == UNIT_PLAYER ? 0 : t == UNIT_MONSTER ? 1 : 2; };
    std::vector<Pending> order;
    for (int r = 0; r < 3; ++r) {
        if (r == 0) { for (const Pending& p : q) if (rank(p.type) == 0) order.push_back(p); }
        else for (auto it = q.rbegin(); it != q.rend(); ++it) if (rank(it->type) == r) order.push_back(*it);
    }
    for (const Pending& p : order) {
        D2UnitStrc* u = SUNIT_GetServerUnit(p.game, int32_t(p.type), int32_t(p.id));
        if (!u) continue;
        // retail's room seed rolls once fewer than our object count before a Druid summon or Firestorm maker picks, while both
        // zombies keep their rolls after the player's (their start facings): the Blood Moor start's loose rock (object 174, no init
        // function) is taken as the one that does not roll. ponytail: the shrine beside it (81) fits every recording equally.
        if (u->dwUnitType == UNIT_OBJECT && u->dwClassId == 174) continue;
        const uint32_t init = roll_room(UNITS_GetRoom(u));
        g_unit[key(u->dwUnitType, u->dwUnitId)] = init;
        if (u->dwUnitType == UNIT_MONSTER) monster_direction(u, init);
    }
}
// The client unit's init seed (+0x28); its seed is {init, 666}.
uint32_t init(uint32_t type, uint32_t id) { flush(); auto it = g_unit.find(key(type, id)); return it == g_unit.end() ? 0 : it->second; }
}
void d2client_unitseed_room(const D2ActiveRoomStrc* r, uint32_t lowSeed) { d2client_unitseed::room(r, lowSeed); }
void d2client_unitseed_created(D2UnitStrc* u) { d2client_unitseed::created(u); }
extern "C" void d2client_unitseed_flush() { d2client_unitseed::flush(); }
