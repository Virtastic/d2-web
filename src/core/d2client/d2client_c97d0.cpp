// 1.13c D2Client+0xC97D0: EAX unit, three stack arguments (row, a1, a2; ret 0xC), no result. Disassembly-derived.
// k = LCG(unit+0x20) & 7 picks the .rdata direction pair (D6DAC dx, D6D8C dy). sel = row s16 +0xEA (< 0: return); with +0xEC
// >= 0 a second LCG step picks +0xEC / +0xEE (s % 3 with +0xEE >= 0, else s & 1). Block {3, unit, 0,0, sel, ux+dx, uy+dy, dy, -dx,
// ..., [12] a2} -> 0xB30F0 r (0: return). Then 0xC316(unit, row[+0x118], a1, a2), v = 0x1330(ecx unit+0x20), r[0x54][0x30] = q:
// q[+4] = q[+8] = 0, q[+0xC] = v << 11; 0xC448(r, ux), 0xC394(r, uy) with the coordinates read again.
#include "cl_env.h"
extern "C" void d2cl_c97d0(uint32_t unit, uint32_t row, uint32_t a1, uint32_t a2, const cl::Env& e) {
    static const int32_t DX[8] = {7, 5, 0, -5, -7, -5, 0, 5};
    static const int32_t DY[8] = {0, 5, 7, 5, 0, -5, -7, -5};
    const uint32_t k = e.Lcg(unit + 0x20) & 7;
    const int32_t ux = (int32_t)e.PD(unit, 0xc3be, 0xc), uy = (int32_t)e.PD(unit, 0xc2ec, 0x10);
    const int32_t dx = DX[k], dy = DY[k];
    int32_t sel = e.R16s(row + 0xea);
    if (sel < 0) return;
    if (e.R16s(row + 0xec) >= 0) {
        if (e.R16s(row + 0xee) >= 0) {
            const uint32_t s = e.Lcg(unit + 0x20) % 3;
            if (s == 0) sel = e.R16s(row + 0xec); else if (s == 1) sel = e.R16s(row + 0xee);
        } else if ((e.Lcg(unit + 0x20) & 1) == 0) sel = e.R16s(row + 0xec);
    }
    uint32_t d[23] = {0};
    d[0] = 3; d[1] = unit; d[4] = (uint32_t)sel; d[5] = (uint32_t)(ux + dx); d[6] = (uint32_t)(uy + dy);
    d[7] = (uint32_t)dy; d[8] = (uint32_t)-dx; d[12] = a2;
    const uint32_t r = e.HB(0xb30f0, (uint32_t)(uintptr_t)d);
    if (!r) return;
    e.H(0xc316, unit, e.R(row + 0x118), a1, a2);
    const uint32_t v = e.H(0x1330, 0, 0, 0, 0, 0, 0, unit + 0x20);
    const uint32_t p = e.R(r + 0x54);
    const uint32_t q = p ? e.R(p + 0x30) : 0;
    if (q) { e.W(q + 4, 0); e.W(q + 8, 0); e.W(q + 0xc, v << 11); }
    e.H(0xc448, r, e.PD(unit, 0xc3be, 0xc));
    e.H(0xc394, r, e.PD(unit, 0xc2ec, 0x10));
}
