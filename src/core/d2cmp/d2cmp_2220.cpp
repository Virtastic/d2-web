// 1.13c D2CMP+0x2220 (stdcall table): double the bucket count. Every node is moved onto a local list
// (retail: 12 bytes of stack; ours: the guest address `loc`), the bucket array is resized (0x2490) and
// each bucket reset to offset 4 / empty, the mask becomes 2n-1, the nodes are re-bucketed by
// (key & mask) at each bucket's tail, and the local list is drained and unlinked.
#include "d2cmp_guest.h"
using namespace d2cmpg;
extern "C" void d2cmp_2490(uint8_t* m, uint32_t arr, uint32_t n, const D2CmpGuestEnv* e);
// 1.13c D2CMP+0x2220
extern "C" void d2cmp_2220(uint8_t* m, uint32_t t, uint32_t loc, const D2CmpGuestEnv* e) {
    uint32_t n = rd(m, t + 0x24) + 1;
    uint32_t newcount = n + n;
    uint32_t L = loc + 4;  // the local list's link
    wr(m, L, L); wr(m, L + 4, ~L); wr(m, loc, 0xddddddddu);
    if (rd(m, loc) != 4) {
        e->f1f30(m, loc);
        wr(m, L, L); wr(m, loc, 4); wr(m, L + 4, ~L);
    }
    for (uint32_t i = 0; i < n; ++i) {
        for (;;) {
            uint32_t node = rd(m, rd(m, t + 0x1c) + i * 0xc + 8);
            if ((int32_t)node <= 0) break;
            uint32_t link = rd(m, loc) + node;
            unlink(m, link);
            uint32_t p = rd(m, L);
            wr(m, link, p);
            wr(m, link + 4, rd(m, p + 4));
            wr(m, p + 4, node);
            wr(m, L, link);
        }
    }
    d2cmp_2490(m, t + 0x14, newcount, e);
    for (uint32_t i = 0; i < newcount; ++i) {
        uint32_t b = rd(m, t + 0x1c) + i * 0xc;
        if (rd(m, b) == 4) continue;
        e->f1f30(m, b);
        wr(m, b + 4, b + 4);
        wr(m, b, 4);
        wr(m, b + 8, ~(b + 4));
    }
    wr(m, t + 0x24, newcount - 1);
    for (;;) {
        uint32_t node = rd(m, L + 4);
        if ((int32_t)node <= 0) break;
        uint32_t b = rd(m, t + 0x1c) + (rd(m, node) & rd(m, t + 0x24)) * 0xc;
        uint32_t link = rd(m, b) + node;
        unlink(m, link);
        uint32_t p = rd(m, b + 4);
        wr(m, link, p);
        wr(m, link + 4, rd(m, p + 4));
        wr(m, p + 4, node);
        wr(m, b + 4, link);
    }
    for (;;) {  // drain (empty here; retail spins while a node's prev is 0)
        uint32_t node = rd(m, L + 4);
        if ((int32_t)node <= 0) break;
        unlink(m, rd(m, loc) + node);
    }
    unlink(m, L);
}
