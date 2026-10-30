// Monster facing on a mode order (1.13c D2Client+0x4DC70, the 0x69/0x6A/0x6B/0x6C handler).
// Retail never takes a direction from the packet; the client turns its unit by the selector:
//   x,y selectors (4, 11, 12, 14, 17, 26, 28): when (x, y) is not (0, 0) and not the unit's own tile,
//     Common #10434 (+0x317F0) -> #10739 (+0x352F0, D2Common_10216)(path, x, y, 0).
//   unit selectors (5, 10, 13, 15, 16, 27, 29): the target is set (D2Client+0xA6040, Common #11129), then
//     D2Client+0xA5FB0 turns toward the target's tile (D2Client+0x1630/0x1660) with #10739(path, x, y, 0).
//   skill actions (the 0x4C/0x4D receivers 0xAF1D0/0xAF160 -> D2Client+0x2A750 -> this handler):
//     0x16 (unit target) resolves the target and reaches 0xA30C0 -> #11129 + 0xA5FB0, the unit turn above;
//     0x15 (coordinates) reaches 0xA22B0 -> #10286 then #10434 toward (x, y), with no own-tile test.
// a4 = 0 at every call site: nNewDirection is set and #10193 PATH_AdjustDirection walks nDirection to it.
// The neutral update (0x6D -> 0xAF2B0 -> selector 7): when the unit already stands within one tile of the
//   packet's (x, y) -- always, single-player -- D2Client+0x4DC30 turns a living monster (mode 1..15, not 12)
//   toward its path target (#10392, the last unit order's target; an x,y order clears it, #11129(unit, 0))
//   through 0xA5FB0. ponytail: the walk to a far (x, y) is not modelled; the server unit is already there.
// Single-player: the server unit is the unit drawn. The order's packet reaches the client on the next frame
// (as every server packet does), so the turn is queued and applied when that frame starts.
#include <cstdint>
#include <vector>
#include <unordered_map>
#include "../d2moo/include/Units/Units.h"
#include "../d2moo/include/Path/Path.h"
#include "../d2moo/include/D2Game/UNIT/SUnit.h"
#include "../d2moo/include/D2DataTbls.h"
#include "../d2moo/include/Units/Monster.h"

namespace { std::unordered_map<uint64_t, uint64_t> g_target; uint64_t key(uint32_t t, uint32_t id) { return uint64_t(t) << 32 | id; } }
static void face(D2UnitStrc* unit, int32_t selector, int32_t x, int32_t y, D2UnitStrc* target) {
    if (!unit || !unit->pDynamicPath) return;
    D2CoordStrc self = {};
    switch (selector) {
    case 7: {
        if (unit->dwAnimMode == 0 || unit->dwAnimMode == 12 || unit->dwAnimMode >= 0x10) return;
        auto it = g_target.find(key(unit->dwUnitType, unit->dwUnitId));
        D2UnitStrc* t = it == g_target.end() ? nullptr : SUNIT_GetServerUnit(unit->pGame, int32_t(it->second >> 32), int32_t(uint32_t(it->second)));
        if (!t || t == unit) return;
        D2CoordStrc to = {};
        UNITS_GetCoords(t, &to);
        D2Common_10216(unit->pDynamicPath, to.nX, to.nY, 0);
        return;
    }
    case 4: case 11: case 12: case 14: case 17: case 26: case 28:
        g_target.erase(key(unit->dwUnitType, unit->dwUnitId));
        UNITS_GetCoords(unit, &self);
        if ((x || y) && (x != self.nX || y != self.nY)) D2Common_10216(unit->pDynamicPath, x, y, 0);
        return;
    case 0x15:
        D2Common_10216(unit->pDynamicPath, x, y, 0);
        return;
    case 5: case 10: case 13: case 15: case 16: case 27: case 29: case 0x16: {
        if (!target || target == unit) return;
        g_target[key(unit->dwUnitType, unit->dwUnitId)] = key(target->dwUnitType, target->dwUnitId);
        D2CoordStrc to = {};
        UNITS_GetCoords(target, &to);
        D2Common_10216(unit->pDynamicPath, to.nX, to.nY, 0);
        return;
    }
    default: return;
    }
}

// The monster attack-sound roll: the order handler (0x4DC70) sets the client unit's mode and calls 0x24490, which
// for a monster remaps the mode through the MonSounds CvtMo slots and sends A1/A2 (4/5) to 0x242B0. 0x242B0 draws
// one value from the client hero seed (r = lo32 % 100) when the attack sound is set, gated for Prb < 100 by the
// 15-tick last-roll stamp (D2Client+0x11C4C4) and the busy test. The draw happens as the order arrives, in the
// frame the server sent it and before that frame's weather pass. The shell plays the sound; it reads r here per unit.
// ponytail: 0x87BD0 (BUSY, a monster sound still playing) is the sound engine's state, not held here: read as idle.
extern "C" uint32_t d2_session_client_rand(uint32_t n);
extern "C" void d2_client_monster_sfx_stamp(uint32_t id, uint32_t now);
namespace {
uint32_t g_frame = 0, g_lastRoll = 0xFFFFFF00u;
struct Roll { uint32_t id, frame; int32_t r; };
std::vector<Roll> g_rolls;
// 1.13c D2Client+0x22A70: the unit's MonSounds record.
const D2MonSoundsTxt* monSounds(D2UnitStrc* u) {
    D2MonsterDataStrc* md = u->pMonsterData;
    D2MonStatsTxt* ms = DATATBLS_GetMonStatsTxtRecord(int32_t(u->dwClassId));
    if (!ms) return nullptr;
    if (md && (md->nTypeFlag & 0x02)) {
        D2SuperUniquesTxt* su = DATATBLS_GetSuperUniquesTxtRecord(int16_t(md->wBossHcIdx));
        if (su && int32_t(su->dwMonSound) > 0) return DATATBLS_GetMonSoundsTxtRecordFromSoundId(int32_t(su->dwMonSound));
    }
    if (md && (md->nTypeFlag & 0x18) && int16_t(ms->wUMonSound) > 0) return DATATBLS_GetMonSoundsTxtRecordFromSoundId(ms->wUMonSound);
    return DATATBLS_GetMonSoundsTxtRecordFromMonsterId(int32_t(u->dwClassId));
}
// 1.13c D2Client+0x24490 (monster branch, modes 4/5) -> D2Client+0x242B0 (the roll).
void attackEntry(D2UnitStrc* u, int32_t mode) {
    if (u->dwUnitType != UNIT_MONSTER) return;
    const D2MonSoundsTxt* p = monSounds(u);
    if (!p) return;
    const uint8_t* b = reinterpret_cast<const uint8_t*>(p);
    for (int k = 0; k < 3; ++k)
        if (mode == b[0x7C + 8 * k] && *reinterpret_cast<const int32_t*>(b + 0x80 + 8 * k) < 0) mode = b[0x7D + 8 * k];
    if (mode != 4 && mode != 5) return;
    const uint32_t* r = reinterpret_cast<const uint32_t*>(b);
    const uint32_t a = mode == 4 ? r[1] : r[7], chance = mode == 4 ? r[3] : r[9];
    if (!a || (chance < 100 && g_frame - g_lastRoll < 15)) return;
    const int32_t v = int32_t(d2_session_client_rand(100));
    if (v < int32_t(chance)) { g_lastRoll = g_frame; d2_client_monster_sfx_stamp(u->dwUnitId, g_frame); }   // a played attack sound stamps the unit, which holds off its idle sound
    g_rolls.push_back({ u->dwUnitId, g_frame, v });
}
}
// The struck-unit sound (a client mode change into GetHit/knockback or death): its normal path
// stamps the unit's sound tick and the shared last-roll tick with the frame the change arrives in.
extern "C" void d2_client_monster_hit_stamp(uint32_t id, uint32_t frame) { g_lastRoll = frame; d2_client_monster_sfx_stamp(id, frame); }
extern "C" void d2_client_attack_roll_reset() { g_frame = 0; g_lastRoll = 0xFFFFFF00u; g_rolls.clear(); }
// The roll 0x242B0 drew for this monster's latest attack entry, oldest first; -1 when none is pending.
extern "C" __attribute__((used, visibility("default"))) int d2_client_attack_roll_take(uint32_t id) {
    for (size_t i = 0; i < g_rolls.size(); ++i)
        if (g_rolls[i].id == id) { const int32_t r = g_rolls[i].r; g_rolls.erase(g_rolls.begin() + i); return r; }
    return -1;
}

namespace {
struct Order { D2GameStrc* game; uint32_t type, id; int32_t selector, x, y; uint32_t ttype, tid; bool hasTarget; int32_t mode; bool rolled; };
std::vector<Order> g_orders;
}
extern "C" void d2_client_monster_order_face(D2UnitStrc* unit, int32_t selector, int32_t x, int32_t y, D2UnitStrc* target) {
    if (!unit || !unit->pGame) return;
    g_orders.push_back({ unit->pGame, unit->dwUnitType, unit->dwUnitId, selector, x, y,
                         target ? target->dwUnitType : 0u, target ? target->dwUnitId : 0u, target != nullptr, int32_t(unit->dwAnimMode), false });
}
// The previous frame's orders, in arrival order.
// This frame's orders, at the client frame's tail: the attack-sound roll as each arrives (before the frame's weather).
extern "C" void d2_client_monster_order_roll() {
    ++g_frame;
    while (!g_rolls.empty() && g_frame - g_rolls.front().frame > 4) g_rolls.erase(g_rolls.begin());
    for (Order& o : g_orders) {
        if (o.rolled) continue;
        o.rolled = true;
        if (D2UnitStrc* u = SUNIT_GetServerUnit(o.game, int32_t(o.type), int32_t(o.id))) attackEntry(u, o.mode);
    }
}
extern "C" void d2_client_monster_order_face_flush() {
    std::vector<Order> q;
    q.swap(g_orders);
    for (const Order& o : q) {
        D2UnitStrc* u = SUNIT_GetServerUnit(o.game, int32_t(o.type), int32_t(o.id));
        D2UnitStrc* t = o.hasTarget ? SUNIT_GetServerUnit(o.game, int32_t(o.ttype), int32_t(o.tid)) : nullptr;
        if (u) face(u, o.selector, o.x, o.y, t);
    }
}
// A walk order (selectors 0, 1, 0x14, 0x17, 0x18: the walk, knockback and run modes) builds the client's path
// when its packet arrives, so the client's turn toward the new path starts a frame after the server's. True
// while such an order for this unit is still queued: the frame's facing step leaves the unit alone.
extern "C" int d2_client_monster_turn_held(const D2UnitStrc* unit) {
    for (const Order& o : g_orders)
        if (o.type == unit->dwUnitType && o.id == unit->dwUnitId && o.game == unit->pGame &&
            (o.selector == 0 || o.selector == 1 || o.selector == 0x14 || o.selector == 0x17 || o.selector == 0x18)) return 1;
    return 0;
}
