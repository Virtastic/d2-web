// Shared seam of the one-file D2Game ports of struct batch 15: the D2Seam of d2game_seam4e.h plus the ten-stack-word
// callee 0xF14F0 (EAX, ECX and 10 stack words, ret 0x28), which D2Seam::call (8 stack words) cannot carry.
#pragma once
#include "d2game_seam4e.h"
struct D2S15 {
    const D2Seam* sm;
    void* ctx;
    // F14F0: w[0..9] are the stack words top first (some are pointers to the caller's locals the callee fills)
    uint32_t (*f14f0)(void* ctx, uint32_t eax, uint32_t ecx, uint32_t* w);
    // hookblk with a MASK (a clear bit records 0 for that word): a block handed to a callee holds a pointer word the capture cannot pin
    void (*blkm)(void* ctx, uint32_t rva, const uint32_t* p, uint32_t n, uint32_t mask);
};
// The unit position helper of the F1xxx walkers: types 2 and 4..5 read the path record (+0x2C: x +0xC, y +0x10), other types call D2Common
// A454 (x) or A3B8 (y) on the path, 0 when the path is null.
static inline uint32_t s15_pos(const D2Seam& s, uint32_t unit, bool y) {
    const int32_t t = (int32_t)d2rd32(unit);
    const uint32_t path = d2rd32(unit + 0x2C);
    if (t == 2 || (t > 3 && t <= 5)) return d2rd32(path + (y ? 0x10 : 0xC));
    return path ? s.call(y ? 0xA3B8 : 0xA454, 0, 0, 0, 0, 0, path) : 0;
}
