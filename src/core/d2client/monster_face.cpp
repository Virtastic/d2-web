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

namespace {
struct Order { D2GameStrc* game; uint32_t type, id; int32_t selector, x, y; uint32_t ttype, tid; bool hasTarget; };
std::vector<Order> g_orders;
}
extern "C" void d2_client_monster_order_face(D2UnitStrc* unit, int32_t selector, int32_t x, int32_t y, D2UnitStrc* target) {
    if (!unit || !unit->pGame) return;
    g_orders.push_back({ unit->pGame, unit->dwUnitType, unit->dwUnitId, selector, x, y,
                         target ? target->dwUnitType : 0u, target ? target->dwUnitId : 0u, target != nullptr });
}
// The previous frame's orders, in arrival order.
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
