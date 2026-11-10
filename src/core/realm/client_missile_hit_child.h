#pragma once

#include <cstdint>

namespace d2::realm {

// A missile whose hit rule is the plain burst (the exploding potion) leaves its first hit sub-missile where it
// stopped, and then a second one when the row names it: the second sub-missile itself, or, when the third names
// a later row, one row drawn from the span of the two with the missile's own seed. The second child is marked
// (unit flag 0x10000). A child is asked for with the stopped missile as its origin unit and flags 0x20 with a
// zero target pair; it takes the stopped missile's skill and level.
struct MissileHitChildren { int first, second; bool rolled; };   // -1 for none

// `roll(n)` draws 0..n-1 from the missile's seed; it is called only when the second child is a draw.
template<class Roll>
constexpr MissileHitChildren missileHitChildren(int sub1, int sub2, int sub3, Roll&& roll) {
    MissileHitChildren out{ -1, -1, false };
    if (sub1 < 0) return out;       // no first child: the rule does nothing at all
    out.first = sub1;
    if (sub2 < 0) return out;
    out.second = sub2;
    if (sub3 > sub2) { out.second = sub2 + static_cast<int>(roll(static_cast<uint32_t>(sub3 - sub2 + 1))); out.rolled = true; }
    return out;
}

} // namespace d2::realm
