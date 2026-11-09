// 1.13c D2Client+0xb9c10 -- missile callback: explosion debris burst and screen shake. ECX = unit, no return.
// p = 0xA60D0(EAX = u), f = 10985(u) (0xC4D2), (x, y) = the unit-path position (path block for types 2/4/5, else 10140/10141 on the path).
// f < 0x6E: f == 0 first sends 0xAFF10(EDX = p; 0x214, x, y, 10240(u), 11029(u)) and n = 8; else n = 8 (f < 0x32), 5 (f < 0x50), 1.
//  n times: m = 11029(u), c = 10240(u); p != 0: the 0x5C-byte block {1, p, .., 0x255 (+0x10), x+2, y+2, .., c (+0x2C), m (+0x30)},
//  v = 0xB30F0(ECX = block); v != 0: three unit-seed LCG steps give a = 50 - lo%100, b = 50 - lo%100, z = (lo & 7) + 2 (unsigned);
//  w = [[v+0x54]+0x30] (each pointer checked): w+4 = a<<11, w+8 = b<<11, w+0xC = 0, then w+0x10 = w+0x14 = 0, w+0x18 = z<<11.
// f == 5: GetTickCount (IAT slot 0xCEF5C) into 0x10B9CC, 0x10B9D0 = 0x258, 0x10B9D4 = 0xFA0, 0x10B9D8 = 0xBB8, 0x11BEFC = 3, then the
// %7 test; else f < 200 and f % 7 == 0 (signed): 0xB39E0(p, x, y). Tail 0xB17C0(ECX = u). gl block: +0..+0xC = 0x10B9CC.., +0x10 = 0x11BEFC.
#include <cstdint>
#include "cl_env.h"
extern "C" void d2c_b9c10(uint32_t u, const cl::Env& e) {
    uint32_t p = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    int32_t f = (int32_t)e.H(0xc4d2, u);
    uint32_t x = e.PD(u, 0xc3be, 0xc);
    uint32_t y = e.PD(u, 0xc2ec, 0x10);
    if (f < 0x6e) {
        int32_t n;
        if (f == 0) {
            uint32_t m = e.H(0xc3fa, u), c = e.H(0xc388, u);
            e.H(0xaff10, 0x214, x, y, c, m, 0, 0, p);
            n = 8;
        } else n = f < 0x32 ? 8 : (f < 0x50 ? 5 : 1);
        for (; n > 0; --n) {
            uint32_t m = e.H(0xc3fa, u), c = e.H(0xc388, u);
            if (!p) continue;
            uint32_t q[23] = {};
            q[0] = 1; q[1] = p; q[4] = 0x255; q[5] = x + 2; q[6] = y + 2; q[11] = c; q[12] = m;
            uint32_t v = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
            if (!v) continue;
            uint32_t a = 50 - e.Lcg(u + 0x20) % 100u;
            uint32_t b = 50 - e.Lcg(u + 0x20) % 100u;
            uint32_t z = (e.Lcg(u + 0x20) & 7u) + 2;
            uint32_t pp = e.R(v + 0x54), w = pp ? e.R(pp + 0x30) : 0;
            if (w) { e.W(w + 4, a << 11); e.W(w + 8, b << 11); e.W(w + 0xc, 0); }
            pp = e.R(v + 0x54); w = pp ? e.R(pp + 0x30) : 0;
            if (w) { e.W(w + 0x10, 0); e.W(w + 0x14, 0); e.W(w + 0x18, z << 11); }
        }
    }
    bool chk;
    if (f == 5) {
        const uint32_t g = e.gl;
        e.W(g, e.H(0xcef5c));
        e.W(g + 4, 0x258); e.W(g + 8, 0xfa0); e.W(g + 0xc, 0xbb8); e.W(g + 0x10, 3);
        chk = true;
    } else chk = f < 200;
    if (chk && f % 7 == 0) e.H(0xb39e0, p, x, y);
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
