// 1.13c D2Client+0x73630 -- skill removed from a unit. EAX = skill, stack unit (ret 4).
#include "cl_env.h"
extern "C" void d2c_73630(int32_t skill /*EAX*/, uint32_t u, const cl::Env& e) {
    if (!u || skill < 0 || skill >= (int32_t)e.R(e.dt + 0xc4)) return;
    uint32_t rec = (uint32_t)skill * 0x3c + e.R(e.dt + 0xbc);
    if (!rec) return;
    uint32_t sk = (uint32_t)skill;
    if (e.H(0xc1fc, u, sk)) {
        uint32_t h = e.H(0xc1c0, u, sk);
        if (h && e.H(0xcb86, h) == sk) { e.H(0xc9d0, u, h); e.H(0xc9c4, h); }
        int32_t w = e.R16s(rec + 0x28);
        if (w >= 0) e.H(0x88a70, u, 0, 0, 0, 0, 0, 0, 0, (uint32_t)w);
    }
    e.H(0xc30a, u, sk, 0);
    if (e.R8(rec + 0x21)) e.H(0x72130, u);
    int32_t s = e.R16s(rec + 0xc);
    if (s >= 0) {
        uint32_t b = (uint32_t)s;
        if (s != -1) {
            uint32_t r = e.H(0xcb80, b);
            b += e.H(0x1380, 0, 0, 0, 0, 0, 0, u + 0x20, 0, 0, r);
        }
        e.H(0x6b300, u, 4, 0, 0xffffffffu, 0xffffffffu, b);
    }
    if (e.R16s(rec + 2) >= 0) e.H(0x727f0, (uint32_t)e.R16s(rec + 2), 0, 0, 0, 0, u);
    if (e.R16s(rec + 4) >= 0) e.H(0x727f0, (uint32_t)e.R16s(rec + 4), 0, 0, 0, 0, u);
    if (e.R8(e.bm + 0xc) & e.R8(rec + 0x10)) e.H(0x734d0, 0, 0, 0, 0, 0, 0, 0, 0, 0, u);
    e.H(0xc5c8, u, 0x6fb84cd0u, 0x59c);
}
