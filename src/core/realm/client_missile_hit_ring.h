#pragma once

#include <cstdint>

namespace d2::realm {

// A missile whose hit rule is the ring (the choking gas potion) bursts into two rings of its first hit
// sub-missile around the place it stopped. Each child is asked for with a step out of a fixed table of 16
// compass steps; the first ring walks the table from entry 0, the second from entry 1, so the two interleave.
// `stride` (the row's second hit number, at least 1) is the first ring's walk and `secondStride` (the first hit
// number) the second ring's; no second ring when it is not positive. The first ring flies at the child row's
// first number and the second at its second, each shifted up by 7 into the 8.8 speed the maker takes as given.
// `loops` (the third hit number), when positive, lengthens a looping child's life by that many sub-loops.
struct MissileRingChild { uint32_t flags; int stepX, stepY, velocity, loops; };

constexpr int kMissileRingStepX[16] = { 0, 1, 2, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1 };
constexpr int kMissileRingStepY[16] = { 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1, 0, 1, 2, 2 };

// Writes the children in creation order; the count (at most 31). Every child starts at the missile (flag 1),
// aims one table step away (flag 2) and carries its own 8.8 speed (flags 4 and 0x10).
constexpr int missileRingChildren(int secondStride, int stride, int loops, int childSpeed1, int childSpeed2,
        MissileRingChild out[31]) {
    const uint32_t flags = loops > 0 ? 0x1Fu : 0x17u;
    const int carried = loops > 0 ? loops : 0;
    if (stride < 1) stride = 1;
    int n = 0;
    for (int i = 0; i < 16; i += stride)
        out[n++] = { flags, kMissileRingStepX[i], kMissileRingStepY[i], childSpeed1 * 128, carried };
    if (secondStride > 0)
        for (int i = 0; i < 15; i += secondStride)
            out[n++] = { flags, kMissileRingStepX[i + 1], kMissileRingStepY[i + 1], childSpeed2 * 128, carried };
    return n;
}

} // namespace d2::realm
