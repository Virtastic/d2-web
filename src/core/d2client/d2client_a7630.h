// 1.13c D2Client+0xA7630, the client player-unit constructor (0x59 assign, 0x15 reassign): its seed and facing rule.
// The new unit's seed starts as D2Common #10936 leaves it, {1, 666}. When the unit has a position and the client act
// has a room there (+0xA5A60), that room's seed (room +0x6C/+0x70) is rolled once; the roll's low dword is the unit's
// init seed (+0x28) and #10614 seeds the unit with {init, 666}. Then +0x191E0 sets the start state: unless the new unit
// IS the current player (D2Client [0x11BBFC]), it rolls the unit seed once more and hands the low six bits to #10706
// (the path direction). When it is the current player, no roll and no facing.
// Shared by the live 0x59/0x15 path (src/core/realm/d2gs_client_state.cpp) and the FN probe src/core/fn/d2client_a7630.cpp.
#pragma once
#include <cstdint>

namespace d2client_a7630 {
struct Seed { uint32_t lo, hi; };
inline uint32_t roll(Seed& s) {
    const uint64_t v = uint64_t(s.lo) * 0x6ac690c5ull + s.hi;
    s = { uint32_t(v), uint32_t(v >> 32) };
    return uint32_t(v);
}
struct Result {
    Seed unit;            // the unit seed (+0x20/+0x24) when the constructor returns
    uint32_t initSeed;    // +0x28, written only when hasInit
    bool hasInit;
    bool facingSet;       // #10706 called (the unit is not the current player)
    uint32_t facing;      // its direction, 0..63
};
// room: the client room's seed (rolled in place), or null when there is no room lookup or it found none.
// isCurrent: the new unit is the one D2Client holds as the current player.
// After the room roll: hasInit = a room was found and initSeed is its roll.
inline Result fromInit(bool hasInit, uint32_t initSeed, bool isCurrent) {
    Result r{};
    r.unit = { 1, 666 };                                                          // #10936
    if (hasInit) { r.initSeed = initSeed; r.hasInit = true; r.unit = { initSeed, 666 }; }   // #10614
    if (!isCurrent) { r.facing = roll(r.unit) & 0x3f; r.facingSet = true; }      // +0x191E0 -> #10706
    return r;
}
inline Result seedAndFacing(Seed* room, bool isCurrent) {
    return room ? fromInit(true, roll(*room), isCurrent) : fromInit(false, 0, isCurrent);
}
}  // namespace d2client_a7630
