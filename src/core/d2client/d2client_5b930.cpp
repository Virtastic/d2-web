// 1.13c D2Client+0x5B930 (343 B): stdcall (off*, y*, toggle*), ret 0xC. One 0x48-byte frame block, zeroed.
// cel = 0x5B120(EAX 7, ECX 0; &key), where key is written into the y argument's own stack slot; 0 returns 0.
// 0x5B5A0(EAX 7, EDX key; *off). Bucket list g10A808[key & 0x7F] (+0xC id, +0xE4 next): a node with id key must
// have +0 == 1 (else fatal 0x5A); its address (0 when absent) rides in EBX into 0x5B530(*off, *y).
// toggle: nonzero -> *y -= toggle, toggle = 0; zero -> toggle = 10, *y += 10. g11C28C = *off, g11C290 = *y.
// mode 7 (and g11C294 = 1) when g11C298 && g11C2A0 && GetTickCount() - g11C2A0 >= 700 (unsigned); else 7 if g11C294, else 1.
// D2Gfx 10041(block, *off, 0x3C, -1, mode, 0); *off += 0x38; return 1.
// bk: the pinned 128-word bucket table; g: the pinned globals from 0x11C28C (28C 290 294 298 29C 2A0).
#include <cstdint>
#include "cl_env.h"
extern "C" uint32_t d2c_5b930(uint32_t off, uint32_t y, uint32_t tog, uint32_t bk, uint32_t g, const cl::Env& e) {
    volatile uint32_t f[18] = {};
    volatile uint32_t key = 0;
    uint32_t cel = e.H(0x5B120, (uint32_t)(uintptr_t)&key, 0, 0, 0, 0, 7, 0);
    f[13] = cel;
    if (!cel) return 0;
    e.H(0x5B5A0, e.R(off), 0, 0, 0, 0, 7, 0, key);
    uint32_t n = e.R(bk + (key & 0x7f) * 4);
    for (; n; n = e.R(n + 0xe4))
        if (e.R(n + 0xc) == key) break;
    e.H(0x5B530, e.R(off), e.R(y), 0, 0, 0, 0, 0, 0, n);
    uint32_t t = e.R(tog);
    if (t) { e.W(y, e.R(y) - t); e.W(tog, 0); }
    else { e.W(tog, 10); e.W(y, e.R(y) + 10); }
    e.W(g + 0, e.R(off));
    e.W(g + 4, e.R(y));
    uint32_t mode;
    if (e.R(g + 0xc) && e.R(g + 0x14) && e.H(0xCEF5C) - e.R(g + 0x14) >= 700u) { e.W(g + 8, 1); mode = 7; }
    else mode = e.R(g + 8) ? 7 : 1;
    uint32_t p = (uint32_t)(uintptr_t)f;
    e.Xs(0);
    e.H(0xCEBF4, p, e.R(off), 0x3c, 0xffffffffu, mode);
    if (e.blk) e.blk(e.ctx, 0xCEBF4, p, 18);
    e.W(off, e.R(off) + 0x38);
    return 1;
}
