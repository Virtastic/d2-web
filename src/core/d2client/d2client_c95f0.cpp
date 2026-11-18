// 1.13c D2Client+0xC95F0: fastcall (ECX owner, EDX table index), one stack argument (ret 4). Disassembly-derived.
// Row as 0xC94E0. n0..n3 = Ordinal 0xC316(owner, row[+0x64 / +0x114 / +0x118 / +0x11C], index, arg); any <= 0 returns 0.
// 0xA1340(ebx owner, EDI -> X, word 0 -> Y) must succeed. Block {1, owner, 0,0, sel, X, Y, ..., [11] index, [12] arg} for
// sel = row s16 +0xEA then +0xEC (each >= 0, no upper bound): r = 0xB30F0; on r, 0xC7E30(ecx n1*n2, edx 0, n1*n3) and,
// with r[0x54][0x30] = q, q[+0x10] = -n3 << 11, q[+0x14] = 0, q[+0x18] = -n2 << 11. Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2cl_c95f0(uint32_t owner, int32_t index, uint32_t arg, const cl::Env& e) {
    if (index < 0 || index >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    const uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)index * 0x23c;
    if (!row) return 0;
    static const uint32_t offs[4] = {0x64, 0x114, 0x118, 0x11c};
    int32_t n[4];
    for (int i = 0; i < 4; ++i) {
        n[i] = (int32_t)e.H(0xc316, owner, e.R(row + offs[i]), (uint32_t)index, arg);
        if (n[i] <= 0) return 0;
    }
    const uint32_t A = (uint32_t)n[1] * (uint32_t)n[2], B2 = (uint32_t)n[1] * (uint32_t)n[3];
    int32_t X = 0, Y = 0;
    e.Ed((uint32_t)(uintptr_t)&X);
    if (!e.H(0xa1340, (uint32_t)(uintptr_t)&Y, 0, 0, 0, 0, 0, 0, 0, owner)) return 0;
    uint32_t d[23] = {0};
    d[0] = 1; d[1] = owner; d[5] = (uint32_t)X; d[6] = (uint32_t)Y; d[11] = (uint32_t)index; d[12] = arg;
    static const uint32_t sel[2] = {0xea, 0xec};
    for (int i = 0; i < 2; ++i) {
        const int32_t s = e.R16s(row + sel[i]);
        if (s < 0) continue;
        d[4] = (uint32_t)s;
        const uint32_t r = e.HB(0xb30f0, (uint32_t)(uintptr_t)d);
        if (!r) continue;
        e.H(0xc7e30, B2, 0, 0, 0, 0, 0, A, 0);
        const uint32_t p = e.R(r + 0x54);
        const uint32_t q = p ? e.R(p + 0x30) : 0;
        if (q) { e.W(q + 0x10, (0u - (uint32_t)n[3]) << 11); e.W(q + 0x14, 0); e.W(q + 0x18, (0u - (uint32_t)n[2]) << 11); }
    }
    return 1;
}
