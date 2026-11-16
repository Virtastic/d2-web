// 1.13c D2Common+0x2DDB0 -- level discard test and room unlink (stdcall(level), ret 4), written from the disassembly with its
// callees behind an env so the FN probe can replay retail's hooked call sequence and production can bind the real ones.
// Offsets (1.13c): level +0x10 room list, +0x1AC next level, +0x1B4 drlg, +0x1D0 id; drlg +0x478 pool, +0x47C level list;
// room +0x08/+0x2C extra allocation, +0x24 next, +0x28 flags, +0x44 status, +0x4C coordinate-node list (node next +0x04).
#include <cstdint>
#include "d2common_2ddb0.h"

// 1.13c D2Common+0x2DDB0
extern "C" int32_t D2Common_2DDB0_LevelDiscard(uint32_t level, const D2Common2DDB0Env* e)
{
    const uint32_t drlg = e->rd32(level + 0x1B4);
    const uint32_t pool = e->rd32(drlg + 0x478);   // read before the first room test, as retail does
    for (uint32_t r = e->rd32(level + 0x10); r; r = e->rd32(r + 0x24))
        if (e->rd8(r + 0x44) < 4 || (e->rd32(r + 0x28) & 0x100000)) return 0;
    const uint32_t ids = e->visIds(drlg, (int32_t)e->rd32(level + 0x1D0));
    const int32_t myId = (int32_t)e->rd32(level + 0x1D0);
    for (uint32_t i = 0; i < 8; ++i) {
        const int32_t id = (int32_t)e->rd32(ids + 4 * i);
        if (!id) continue;
        uint32_t vl = e->rd32(e->rd32(level + 0x1B4) + 0x47C);
        while (vl && (int32_t)e->rd32(vl + 0x1D0) != id) vl = e->rd32(vl + 0x1AC);
        if (!vl) vl = e->allocLevel(e->rd32(level + 0x1B4), id);
        const uint32_t first = e->rd32(vl + 0x10);
        if (!first) continue;
        const uint32_t link = e->visIds(e->rd32(vl + 0x1B4), (int32_t)e->rd32(vl + 0x1D0));
        uint32_t mask = 0;
        for (uint32_t k = 0; k < 8; ++k)
            if ((int32_t)e->rd32(link + 4 * k) == myId) mask |= 0x10u << k;
        for (uint32_t r = first; r; r = e->rd32(r + 0x24))
            if ((mask & e->rd32(r + 0x28)) && (e->rd8(r + 0x44) < 4 || (e->rd32(r + 0x28) & 0x100000))) return 0;
        for (uint32_t r = first; r; r = e->rd32(r + 0x24)) {
            if (!(e->rd32(r + 0x28) & mask)) continue;
            for (uint32_t n = e->rd32(r + 0x4C); n;) { const uint32_t nx = e->rd32(n + 4); e->freePool(pool, n, 0x58); n = nx; }
            e->wr32(r + 0x4C, 0);
            if (e->rd32(r + 8)) { e->freePool(pool, e->rd32(r + 8), 0x16F); e->wr32(r + 8, 0); e->wr32(r + 0x2C, 0); }
        }
    }
    return 1;
}
