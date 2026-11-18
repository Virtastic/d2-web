// 1.13c D2Client+0x15C40: fastcall (ECX owner, EDX table index), one stack argument (ret 4). Disassembly-derived.
// Row = *(dt+0xB98) + index * 0x23C (index in [0, dt+0xBA0); a zero row returns 0). owner[0xC4] |= 0x40 (owner may be 0).
// 0xA1340(ebx owner, EDI -> X, stack word 0 -> Y) must succeed; first = 0x40550(eax index; owner) in [0, dt+0xB6C); t = 0xA5F70().
// 0xB30F0 takes a 23-dword block {t ? 0x20 : 0x420, owner, owner, t, first, 0,0, X, Y, ..., [11] index, [12] arg} and returns r (0: done, return 1).
// Else 0xC448(r, t ? 1 : 2), x = X - 0x1630(ecx owner), y = Y - 0x1660(ecx owner), 0xC394(r, (s16(y) << 16) + s16(x)). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2cl_15c40(uint32_t owner, int32_t index, uint32_t arg, const cl::Env& e) {
    if (index < 0 || index >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    if (!(e.R(e.dt + 0xb98) + (uint32_t)index * 0x23c)) return 0;
    if (owner) e.W(owner + 0xc4, e.R(owner + 0xc4) | 0x40);
    int32_t X = 0, Y = 0;
    e.Ed((uint32_t)(uintptr_t)&X);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&Y, 0, 0, 0, 0, 0, 0, 0, owner)) return 0;
    const int32_t first = (int32_t)e.H(0x40550, owner, 0, 0, 0, 0, (uint32_t)index);
    if (first < 0 || first >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    const uint32_t t = e.H(0xa5f70);
    uint32_t d[23] = {0};
    d[0] = t ? 0x20 : 0x420; d[1] = owner; d[2] = owner; d[3] = t; d[4] = (uint32_t)first;
    d[7] = (uint32_t)X; d[8] = (uint32_t)Y; d[11] = (uint32_t)index; d[12] = arg;
    const uint32_t r = e.HB(0xb30f0, (uint32_t)(uintptr_t)d);
    if (!r) return 1;
    e.H(0xc448, r, t ? 1 : 2);
    const int32_t x = X - (int32_t)e.H(0x1630, 0, 0, 0, 0, 0, 0, owner);
    const int32_t y = Y - (int32_t)e.H(0x1660, 0, 0, 0, 0, 0, 0, owner);
    e.H(0xc394, r, (uint32_t)(((int32_t)(int16_t)y << 16) + (int32_t)(int16_t)x));
    return 1;
}
