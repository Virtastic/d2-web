// 1.13c D2Client+0xbc210 -- missile callback: per-frame tick dispatch. ECX = unit, no return.
// Invalid Missiles row: 0xB1720(ESI = u). p = 0xA60D0(EAX = u); p == 0 tails into 0xB17C0(ECX = u). f = 10985(u) (frame).
//  s16 +0x1e > 0 and f > 10: 0xBB5B0(p, u, s16 +0x1e, f, 0xB0000(EAX = f, ECX = row)).
//  s16 +0x20 > 0 and f % max(1, row+0x58) == 0: 0xB82B0(EAX = u; p, s16 +0x20, 0x14).
//  row+0x5c = c > 0 and f == 0x5A: q = c/3, 0x89F00(ESI = (c-2q)*25000/25; 8, q*25000/25, q*25000/25) (32-bit wrap, signed /).
//  s16 +0x22 = h > 0 and 0x73 < f < 0x13B: 0xBB570(EAX = f, EBX = p, ESI = h; EDI = u).
// Tail 0xB17C0(ECX = u).
#include <cstdint>
#include "cl_env.h"
extern "C" void d2c_bc210(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    uint32_t p = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!p) { e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u); return; }
    int32_t f = (int32_t)e.H(0xc4d2, u);
    int32_t s = e.R16s(row + 0x1e);
    if (s > 0 && f > 10) {
        uint32_t t = e.H(0xb0000, 0, 0, 0, 0, 0, (uint32_t)f, row);
        uint32_t w[5] = { p, u, (uint32_t)s, (uint32_t)f, t };
        e.H(0xbb5b0, p, u, (uint32_t)s, (uint32_t)f, t);
        if (e.blk) e.blk(e.ctx, 0xbb5b0, (uint32_t)(uintptr_t)w, 5);
    }
    int32_t d = (int32_t)e.R(row + 0x58); if (d < 1) d = 1;
    int32_t w20 = e.R16s(row + 0x20);
    if (w20 > 0 && f % d == 0) e.H(0xb82b0, p, (uint32_t)w20, 0x14, 0, 0, u);
    int32_t c = (int32_t)e.R(row + 0x5c);
    if (c > 0 && f == 0x5a) {
        int32_t q = c / 3;
        int32_t hi = (int32_t)((uint32_t)q * 25000u) / 25;
        int32_t lo = (int32_t)((uint32_t)(c - 2 * q) * 25000u) / 25;
        e.H(0x89f00, 8, (uint32_t)hi, (uint32_t)hi, 0, 0, 0, 0, 0, 0, (uint32_t)lo);
    }
    int32_t h = e.R16s(row + 0x22);
    if (h > 0 && f > 0x73 && f < 0x13b) e.H(0xbb570, 0, 0, 0, 0, 0, (uint32_t)f, 0, 0, p, (uint32_t)h);
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
