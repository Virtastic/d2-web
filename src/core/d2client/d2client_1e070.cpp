// 1.13c D2Client+0x1E070 -- burst spawn: EAX in (its low byte is cleared on the idle path), stdcall(x, y), ret 8.
// gl block: +3 = enable byte D2Client+0x12339F, +4 = byte 0x1233A0, +5 = dword 0x1233A1 (unaligned), +12 = the spawn counter
// 0xDE834, +16 = the client state slot 0x11BBFC. Enabled: one LCG step on the state's 64-bit seed (+0x20 lo, +0x24 hi), then
// (lo % 3) + 2 spawns through 0xA7780(&rec, 0, 0, 0, 0, 1) with rec = {++counter (wraps -1 to 0), 0x95, x + (i & 1),
// y + (i & 1), 0, 0, 1}; each stores the spawned object's +0xC (or -1) at 0x1233A1 and 1 at 0x1233A0. Returns the last store.
#include "cl_env.h"
extern "C" uint32_t d2c_1e070(uint32_t eax, uint32_t x, uint32_t y, const cl::Env& e) {
    const uint32_t g = e.gl;
    if (!e.R8(g + 3)) return eax & ~0xffu;
    uint32_t st = e.R(g + 16);
    uint64_t s = (uint64_t)e.R(st + 0x20) * 0x6ac690c5u + e.R(st + 0x24);
    e.W(st + 0x24, (uint32_t)(s >> 32)); e.W(st + 0x20, (uint32_t)s);
    uint32_t n = (uint32_t)s % 3 + 2, r = 0;
    for (uint32_t i = 0; i < n; ++i) {
        uint32_t c = e.R(g + 12) + 1;
        if (c == 0xffffffffu) c = 0;
        e.W(g + 12, c);
        uint32_t rec[7] = {c, 0x95, x + (i & 1), y + (i & 1), 0, 0, 1};
        uint32_t p = (uint32_t)(uintptr_t)rec;
        uint32_t o = e.H(0xa7780, p, 0, 0, 0, 0);
        if (e.blk) e.blk(e.ctx, 0xa7780, p, 7);
        r = o ? e.R(o + 0xc) : 0xffffffffu;
        e.W(g + 5, r);
        e.W(g + 4, (e.R(g + 4) & ~0xffu) | 1);
    }
    return r;
}
