// Host test: E=$(mktemp -d) && echo '#define EMSCRIPTEN_KEEPALIVE' > $E/emscripten.h &&
//   g++ -std=c++17 -I$E -Isrc/core/d2moo/compat -Isrc/core/d2moo/include -Isrc/core/d2moo/include/D2Game
//   -o $E/t src/core/realm/d2gs_client_state_stats.test.cpp && $E/t
// A town NPC is linked into its room before its alignment state packet arrives, then the hero
// leaves the game and every unit is unlinked. The room's friendly count must end at zero
// without the unlink ever taking it below zero (retail exits on that assert).
#include "d2gs_client_state_stats.cpp"
#include <D2States.h>
#include <cassert>
#include <map>
#include <vector>

// Fake D2Common: one stat list per (unit, state), a room count, and the link/unlink rule.
struct FakeList { std::map<int, int> stats; };
static std::map<std::pair<D2UnitStrc*, int>, FakeList*> g_lists;
static std::map<D2UnitStrc*, D2ActiveRoomStrc*> g_roomOf;
static std::map<D2ActiveRoomStrc*, int> g_allies;
static int g_footprints;
static FakeList* g_pending;

D2StatListStrc* __stdcall STATLIST_GetStatListFromUnitAndState(D2UnitStrc* u, int s) {
    auto it = g_lists.find({u, s});
    return it == g_lists.end() ? nullptr : reinterpret_cast<D2StatListStrc*>(it->second);
}
D2StatListStrc* __stdcall STATLIST_AllocStatList(void*, uint32_t, uint32_t, int, D2UnitGUID) {
    g_pending = new FakeList();
    return reinterpret_cast<D2StatListStrc*>(g_pending);
}
static int g_pendingState = -1;
void __stdcall STATLIST_SetState(D2StatListStrc*, int s) { g_pendingState = s; }
void __stdcall D2COMMON_10475_PostStatToStatList(D2UnitStrc* u, D2StatListStrc*, BOOL) {
    g_lists[{u, g_pendingState}] = g_pending;
}
int __stdcall STATLIST_GetStatValue(D2StatListStrc* l, int id, uint16_t) {
    auto& m = reinterpret_cast<FakeList*>(l)->stats;
    auto it = m.find(id);
    return it == m.end() ? 0 : it->second;
}
BOOL __stdcall STATLIST_SetStat(D2StatListStrc* l, int id, int v, uint16_t) {
    reinterpret_cast<FakeList*>(l)->stats[id] = v;
    return TRUE;
}
D2ActiveRoomStrc* __stdcall UNITS_GetRoom(D2UnitStrc* u) { return g_roomOf.count(u) ? g_roomOf[u] : nullptr; }
void __stdcall DUNGEON_IncreaseAlliedCountOfRoom(D2ActiveRoomStrc* r) { ++g_allies[r]; }
void __stdcall DUNGEON_DecreaseAlliedCountOfRoom(D2ActiveRoomStrc* r) { assert(g_allies[r] > 0); --g_allies[r]; }
void __stdcall D2Common_10214(D2UnitStrc*) { ++g_footprints; }

static bool good(D2UnitStrc* u) { // STATLIST_GetUnitAlignment == good, as the engine reads it
    if (u->dwUnitType == UNIT_PLAYER) return true;
    FakeList* l = reinterpret_cast<FakeList*>(STATLIST_GetStatListFromUnitAndState(u, STATE_ALIGNMENT));
    return l && l->stats.count(STAT_ALIGNMENT) && l->stats[STAT_ALIGNMENT] == UNIT_ALIGNMENT_GOOD;
}
static void link(D2UnitStrc* u, D2ActiveRoomStrc* r) { g_roomOf[u] = r; if (good(u)) ++g_allies[r]; }
static void unlink(D2UnitStrc* u) { D2ActiveRoomStrc* r = g_roomOf[u]; if (good(u)) DUNGEON_DecreaseAlliedCountOfRoom(r); g_roomOf.erase(u); }

int main() {
    alignas(16) static uint8_t roomMem[0x100], npcMem[sizeof(D2UnitStrc)], npc2Mem[sizeof(D2UnitStrc)],
        heroMem[sizeof(D2UnitStrc)];
    auto* room = reinterpret_cast<D2ActiveRoomStrc*>(roomMem);
    auto* npc = reinterpret_cast<D2UnitStrc*>(npcMem);
    auto* npc2 = reinterpret_cast<D2UnitStrc*>(npc2Mem);
    auto* hero = reinterpret_cast<D2UnitStrc*>(heroMem);
    npc->dwUnitType = UNIT_MONSTER; npc2->dwUnitType = UNIT_MONSTER; hero->dwUnitType = UNIT_PLAYER;
    const D2RealmClientStateStat goodAlign{STAT_ALIGNMENT, 0, UNIT_ALIGNMENT_GOOD};
    const D2RealmClientStateStat evilAlign{STAT_ALIGNMENT, 0, UNIT_ALIGNMENT_EVIL};

    link(hero, room);
    link(npc, room);                       // no alignment yet: not counted
    link(npc2, room);
    assert(g_allies[room] == 1);
    // 0xA8 for npc: the state's list is new, previous alignment is "none" (4): +1.
    assert(d2gs_client_apply_state_stats(npc, STATE_ALIGNMENT, &goodAlign, 1));
    assert(g_allies[room] == 2 && g_footprints == 1);
    // 0xAA repeating the same alignment on the existing list: unchanged, no footprint rebuild.
    assert(d2gs_client_apply_state_stats(npc, STATE_ALIGNMENT, &goodAlign, 1));
    assert(g_allies[room] == 2 && g_footprints == 1);
    // 0xAA for npc2: good, then (existing list) turned evil: +1 then -1.
    assert(d2gs_client_apply_state_stats(npc2, STATE_ALIGNMENT, &goodAlign, 1));
    assert(g_allies[room] == 3);
    assert(d2gs_client_apply_state_stats(npc2, STATE_ALIGNMENT, &evilAlign, 1));
    assert(g_allies[room] == 2 && g_footprints == 3);
    // Leaving the game unlinks everyone; the count lands on zero with no assert.
    unlink(npc); unlink(npc2); unlink(hero);
    assert(g_allies[room] == 0);

    // A unit with no room (not linked yet) moves no count; a player's alignment moves no count.
    assert(d2gs_client_apply_state_stats(npc2, STATE_ALIGNMENT, &goodAlign, 1));
    link(hero, room);
    d2gs_client_alignment_changed(hero, 4, UNIT_ALIGNMENT_GOOD);
    assert(g_allies[room] == 1);
    link(npc2, room); unlink(npc2); unlink(hero);
    assert(g_allies[room] == 0);
    return 0;
}
