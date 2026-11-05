// 1.13c D2Client+0xC8B40: fastcall (ECX owner, EDX table index), one stack argument (ret 4). Disassembly-derived.
// Row as 0xC94E0; sel = row s16 +0xEA must lie in [0, dt+0xB6C) (else 0). owner[0xC4] |= 0x40 (owner may be 0).
// 0xA1340(ebx owner, EDI -> X, word 0 -> Y) must succeed. n = max(2, 0xC316(owner, row[+0x64], index, arg) - 3).
// Block {1, owner, 0,0, 0xBF, X, Y, ..., [11] index, [12] arg} -> 0xB30F0 r (0: return 0). 0xa5420(); m = 0xA8C20(ebx 1;
// 1, 0xFF, 0xFF, 0, 0); on m: 0xA8BE0(eax n, ecx m), r[0x64] = m. 0xCC76(r, sel), 0xCC7C(r, n). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2cl_c8b40(uint32_t owner, int32_t index, uint32_t arg, const cl::Env& e) {
    if (index < 0 || index >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    const uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)index * 0x23c;
    if (!row) return 0;
    const int32_t sel = e.R16s(row + 0xea);
    if (sel < 0 || sel >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    if (owner) e.W(owner + 0xc4, e.R(owner + 0xc4) | 0x40);
    int32_t X = 0, Y = 0;
    e.Ed((uint32_t)(uintptr_t)&X);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&Y, 0, 0, 0, 0, 0, 0, 0, owner)) return 0;
    int32_t n = (int32_t)(e.H(0xc316, owner, e.R(row + 0x64), (uint32_t)index, arg) - 3);
    if (n < 2) n = 2;
    uint32_t d[23] = {0};
    d[0] = 1; d[1] = owner; d[4] = 0xbf; d[5] = (uint32_t)X; d[6] = (uint32_t)Y; d[11] = (uint32_t)index; d[12] = arg;
    const uint32_t r = e.HB(0xb30f0, (uint32_t)(uintptr_t)d);
    if (!r) return 0;
    e.H(0xa5420);
    const uint32_t m = e.H(0xa8c20, 1, 0xff, 0xff, 0, 0, 0, 0, 0, 1);
    if (m) { e.H(0xa8be0, 0, 0, 0, 0, 0, (uint32_t)n, m); e.W(r + 0x64, m); }
    e.H(0xcc76, r, (uint32_t)sel);
    e.H(0xcc7c, r, (uint32_t)n);
    return 1;
}
