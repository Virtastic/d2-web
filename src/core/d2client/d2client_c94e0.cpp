// 1.13c D2Client+0xC94E0: fastcall (ECX owner, EDX table index), one stack argument (ret 4). Disassembly-derived.
// Row = *(dt+0xB98) + index * 0x23C (index in [0, dt+0xBA0); a zero row returns 0). 0xA1340(ebx owner, EDI -> X, word 0 -> Y)
// must succeed. Block {1, owner, 0,0, sel, X, Y, ..., [11] index, [12] arg}: sel = row s16 +0xEA then +0xEC, each sent through
// 0xB30F0 when in [0, dt+0xB6C); a non-zero result of the second gets 0xC448(r, owner[0x28]). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2cl_c94e0(uint32_t owner, int32_t index, uint32_t arg, const cl::Env& e) {
    if (index < 0 || index >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    const uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)index * 0x23c;
    if (!row) return 0;
    int32_t X = 0, Y = 0;
    e.Ed((uint32_t)(uintptr_t)&X);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&Y, 0, 0, 0, 0, 0, 0, 0, owner)) return 0;
    uint32_t d[23] = {0};
    d[0] = 1; d[1] = owner; d[5] = (uint32_t)X; d[6] = (uint32_t)Y; d[11] = (uint32_t)index; d[12] = arg;
    const int32_t a = e.R16s(row + 0xea);
    if (a >= 0 && a < (int32_t)e.R(e.dt + 0xb6c)) { d[4] = (uint32_t)a; e.HB(0xb30f0, (uint32_t)(uintptr_t)d); }
    const int32_t b = e.R16s(row + 0xec);
    if (b >= 0 && b < (int32_t)e.R(e.dt + 0xb6c)) {
        d[4] = (uint32_t)b;
        const uint32_t r = e.HB(0xb30f0, (uint32_t)(uintptr_t)d);
        if (r) e.H(0xc448, r, e.R(owner + 0x28));
    }
    return 1;
}
