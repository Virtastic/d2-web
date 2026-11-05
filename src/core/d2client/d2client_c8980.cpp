// 1.13c D2Client+0xC8980: EAX unit, three stack arguments a0..a2 (ret 0xC). Disassembly-derived.
// 0xA1340(ebx unit, EDI -> X, word 0 -> Y) must succeed (else 0). dX = X - ux, dY = Y - uy, n = max(1, 0xC4D8(unit, X, Y)).
// Block {3, unit, 0,0, a2, 2dX/n + ux, 2dY/n + uy, 5dX, 5dY, ..., [11] a0, [12] a1} (coordinates read again, signed division)
// -> 0xB30F0 r (0: return 0); 0xC2B6(r[0x2C], 0xCC82(unit, X, Y)). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2cl_c8980(uint32_t unit, uint32_t a0, uint32_t a1, uint32_t a2, const cl::Env& e) {
    int32_t X = 0, Y = 0;
    e.Ed((uint32_t)(uintptr_t)&X);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&Y, 0, 0, 0, 0, 0, 0, 0, unit)) return 0;
    const int32_t dX = (int32_t)((uint32_t)X - e.PD(unit, 0xc3be, 0xc));
    const int32_t dY = (int32_t)((uint32_t)Y - e.PD(unit, 0xc2ec, 0x10));
    int32_t n = (int32_t)e.H(0xc4d8, unit, (uint32_t)X, (uint32_t)Y);
    if (n <= 1) n = 1;
    const uint32_t nx = (uint32_t)((int32_t)((uint32_t)dX * 2) / n) + e.PD(unit, 0xc3be, 0xc);
    const uint32_t uy = e.PD(unit, 0xc2ec, 0x10);
    const uint32_t ny = (uint32_t)((int32_t)((uint32_t)dY * 2) / n) + uy;
    uint32_t d[23] = {0};
    d[0] = 3; d[1] = unit; d[4] = a2; d[5] = nx; d[6] = ny; d[7] = (uint32_t)dX * 5; d[8] = (uint32_t)dY * 5; d[11] = a0; d[12] = a1;
    const uint32_t r = e.HB(0xb30f0, (uint32_t)(uintptr_t)d);
    if (!r) return 0;
    const uint32_t v = e.H(0xcc82, unit, (uint32_t)X, (uint32_t)Y);
    e.H(0xc2b6, e.R(r + 0x2c), v);
    return 1;
}
