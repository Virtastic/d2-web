// 1.13c D2Client+0xb4b80 -- client missile: the burst sequence of a counting unit, then its tick (0xB17C0).
// ECX = unit. n = D2Common counter (0xC406); n == 0x96 arms the screen shake (0x10B9CC..0x10B9D8, 0x11BEFC) from
// GetTickCount. At n = 325, 300, .., 175 one class-0x154 piece spawns at the step table 0xD36D0 offset (dx, dy, dir)
// with direction `dir` (0xC3C4 on its path) and n as its frame data (0xC4A2 / 0xC2C8); every even n < 0xA5 a class-0x153
// riser (rise 0x23000, fall 0x800, path dir 0x15); every n < 0x96 a class-0x96 ember at a jittered offset (0xC2FE 0x40).
// gl: +0x40 = the four dwords at 0x10B9CC, +0x50 = the dword at 0x11BEFC.
#include "cl_env.h"
namespace {
const int32_t kStep[7][3] = { { 3, -2, 4 }, { 1, -3, 58 }, { 3, 1, 13 }, { -2, -3, 49 }, { 1, 3, 22 }, { -3, 0, 40 }, { -2, 2, 31 } };   // 0xD36D0
}
extern "C" void d2c_b4b80(uint32_t u, uint32_t gl, uint32_t tick, const cl::Env& e) {
    int32_t n = (int32_t)e.H(0xc406, u);
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (n == 0x96) {
        e.W(gl + 0x40, tick); e.W(gl + 0x44, 0); e.W(gl + 0x48, 0xfa0); e.W(gl + 0x4c, 0);
        e.W(gl + 0x50, 0x19);
    }
    for (int k = 0; k < 7; ++k) {
        int32_t t = 0x145 - 25 * k;
        if (n > t) break;
        if (n != t) continue;
        uint32_t py = e.PD(u, 0xc2ec, 0x10);
        uint32_t px = e.PD(u, 0xc3be, 0xc);
        uint32_t b = e.H(0xc3fa, u);
        uint32_t a = e.H(0xc388, u);
        if (!room) continue;
        uint32_t q[23] = {};
        q[0] = 1; q[1] = room; q[4] = 0x154;
        q[5] = (uint32_t)kStep[k][0] + px;
        q[6] = (uint32_t)kStep[k][1] + py;
        q[11] = a; q[12] = b;
        uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (!m) continue;
        e.H(0xc3c4, e.R(m + 0x2c), (uint32_t)kStep[k][2] & 0xff);
        e.H(0xc4a2, m, (uint32_t)n);
        e.H(0xc2c8, m, (uint32_t)n);
    }
    if (n < 0xa5 && n % 2 == 0) {
        uint32_t q[23] = {};
        q[0] = 0x21; q[1] = room;
        q[5] = e.PD(u, 0xc3be, 0xc);
        q[6] = e.PD(u, 0xc2ec, 0x10);
        q[7] = e.PD(u, 0xc3be, 0xc) - 13;
        q[8] = e.PD(u, 0xc2ec, 0x10) + 3;
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        q[4] = 0x153;
        uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (m) {
            uint32_t pa = e.R(m + 0x54);
            if (pa) { uint32_t mo = e.R(pa + 0x30); if (mo) { e.W(mo + 4, 0); e.W(mo + 8, 0); e.W(mo + 0xc, 0x23000); } }
            pa = e.R(m + 0x54);
            if (pa) { uint32_t mo = e.R(pa + 0x30); if (mo) { e.W(mo + 0x10, 0); e.W(mo + 0x14, 0); e.W(mo + 0x18, 0xfffff800u); } }
            uint32_t p = e.R(m + 0x2c);
            e.H(0xc3c4, p, 0x15);
            e.H(0xc3e8, p, 0);
        }
    }
    if (n < 0x96) {
        uint32_t x = e.PD(u, 0xc3be, 0xc) - 13;
        uint32_t y = e.PD(u, 0xc2ec, 0x10) + 3;
        uint32_t q[23] = {};
        q[0] = 0x21; q[1] = room; q[4] = 0x96;
        q[5] = x; q[6] = y;
        q[7] = e.Lcg(u + 0x20) % 5 + x + 1;
        q[8] = e.Lcg(u + 0x20) % 10 + y - 5;
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        if (m) e.H(0xc2fe, m, 0x40);
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
