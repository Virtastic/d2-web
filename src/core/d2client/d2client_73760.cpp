// 1.13c D2Client+0x73760 -- skill set on a unit. EAX = unit, stack skill (ret 4).
#include "cl_env.h"
extern "C" void d2c_73760(uint32_t u /*EAX*/, int32_t skill, const cl::Env& e) {
    if (!u || skill < 0 || skill >= (int32_t)e.R(e.dt + 0xc4)) return;
    uint32_t rec = (uint32_t)skill * 0x3c + e.R(e.dt + 0xbc);
    if (!rec) return;
    uint32_t sk = (uint32_t)skill;
    if (e.R8(e.bm + 0x1c) & e.R8(rec + 0x14)) {
        uint32_t k = e.R(u);
        if ((k == 1 && e.R(u + 0x10) == 0xc) || (k == 0 && e.R(u + 0x10) == 0x11)) { e.H(0xc30a, u, sk, 1); return; }
    }
    if (e.H(0xc1fc, u, sk)) {
        uint32_t h = e.H(0xc1c0, u, sk);
        if (!(e.R8(e.bm + 0x10) & e.R8(rec + 0x14))) e.H(0xcba4, h);
    } else {
        int32_t w = e.R16s(rec + 0x26);
        if (w >= 0) e.H(0x88a70, u, 0, 0, 0, 0, 0, 0, 0, (uint32_t)w);
    }
    e.H(0xc30a, u, sk, 1);
    if (e.R8(rec + 0x21)) e.H(0x72130, u);
    e.H(0x72c80, sk, 0, 0, 0, 0, 0, 0, 0, u);
    if (e.H(0xa5440, 0, 0, 0, 0, 0, u) && e.H(0xcbb0, u)) {
        uint32_t f = e.R(u + 0xc4), g = e.R(u + 0xc8) | 0x40000;
        e.W(u + 0xc8, g);
        e.W(u + 0xc4, (f & 0xfffffffdu) | 0x20);
    }
    if (e.R8(e.bm + 0xc) & e.R8(rec + 0x10)) e.H(0x734d0, 0, 0, 0, 0, 0, 0, 0, 0, 0, u);
}
