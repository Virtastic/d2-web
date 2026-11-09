// 1.13c D2Client+0x84c60 -- free the items a unit owns. EDI = unit (null ends it); inventory = unit +0x60 (null ends it); no stack arguments.
// Walk 0xC4F0 (first) / 0xC568 (next, read before the item is touched): the item's 0xC526 handle (null is fatal), then the kind 0xC994: 3, or 1
// with 0xCA30(handle, unit) != 0, is released: a type-0 unit whose +0x10 is not 0 / 0x11 and whose flag word +0xC4 lacks 0x10000 runs 0x19830
// (ECX = 7, EDX = unit, stack 0, 1); 0xCA2A, 0xC6D0 must hand the handle back (null / another is fatal), kind 3 again runs 0xC9E8 + 0xCAA8
// (ebx has become the inventory), 0xCA4E, 0xCA0C(unit +0x60) for a type-0 unit, 0xC1A8(item, 0x100, 0x96, file) == 0 runs 0xC544, 0x82FE0.
// The walk ends with 0xC9E2(inventory).
#include "cl_env.h"
extern "C" void d2c_84c60(uint32_t u, const cl::Env& e) {
    if (!u) return;
    const uint32_t inv = e.R(u + 0x60);
    if (!inv) return;
    uint32_t cur = e.H(0xc4f0, inv);
    while (cur) {
        const uint32_t next = e.H(0xc568, cur);
        const uint32_t h = e.H(0xc526, cur);
        if (!h) return;                                        // fatal in retail
        bool go = e.H(0xc994, cur) == 3;
        if (!go && e.H(0xc994, cur) == 1 && e.H(0xca30, h, u)) go = true;
        if (go) {
            if (!(e.R(u + 0xc4) & 0x10000)) {
                const uint32_t ty = e.R(u), f = e.R(u + 0x10);
                if (ty == 0 && f != 0 && f != 0x11) e.H(0x19830, 0, 1, 0, 0, 0, 0, 7, u);
            }
            e.H(0xca2a, inv, h);
            const uint32_t g = e.H(0xc6d0, inv, h);
            if (!g || g != h) return;                          // fatal in retail
            if (e.H(0xc994, cur) == 3) { const uint32_t r = e.H(0xc9e8, inv, h); e.H(0xcaa8, inv, r); }
            e.H(0xca4e, u, g, 1, 1);
            if (e.R(u) == 0) e.H(0xca0c, e.R(u + 0x60));
            if (!e.H(0xc1a8, g, 0x100, 0x96, 0xB2000000u | 0xD4964u)) e.H(0xc544, u, g);
            e.H(0x82fe0, u);
        }
        cur = next;
    }
    e.H(0xc9e2, inv);
}
