// 1.13c D2Client+0xC52E0: fastcall(ECX = unit, EDX = skill index) + 1 stack word a3 (ret 4).
// Skill row (sgptDataTables+0xB98, stride 0x23C, count +0xBA0) missing returns 0. mi = #40550(EAX = index, stack unit) outside [0, count at +0xB6C) or its
// missile row (stride 0x1A4 at +0xB64) null returns 0. #C45A(unit, &A, &B) fills A and B; both zero returns 0. o = #A1540(EAX = unit, stack mi, index, a3) == 0
// returns 0. m = max(short missile row+0x96, 1); n = #C316(unit, skill row+0x114, index, a3); P = -m*A, Q = -m*B, #C2F8(&P, &Q); S2 = A, S1 = B, #C2F8(&S2, &S1);
// #C7E30(EAX = o, ECX = n*m, EDX = Q, stack P); if o word +0x54 and its word +0x30 are non-null: [+0x10] = S2 << 11, [+0x14] = S1 << 11, [+0x18] = -n << 11.
// #C7FB0(o). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_c52e0(uint32_t unit, int32_t cls, uint32_t a3, const cl::Env& e) {
    if (cls < 0 || cls >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t row = e.R(e.dt + 0xb98) + (uint32_t)cls * 0x23c;
    if (!row) return 0;
    int32_t mi = (int32_t)e.H(0x40550, unit, 0, 0, 0, 0, (uint32_t)cls);
    if (mi < 0 || mi >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    uint32_t mrow = e.R(e.dt + 0xb64) + (uint32_t)mi * 0x1a4;
    if (!mrow) return 0;
    uint32_t ab[2] = {};   // [0] = A, [1] = B
    e.H(0xc45a, unit, (uint32_t)(uintptr_t)&ab[0], (uint32_t)(uintptr_t)&ab[1]);
    if (!ab[0] && !ab[1]) return 0;
    uint32_t o = e.H(0xa1540, (uint32_t)mi, (uint32_t)cls, a3, 0, 0, unit);
    if (!o) return 0;
    int32_t m = e.R16s(mrow + 0x96);
    if (m < 1) m = 1;
    uint32_t n = e.H(0xc316, unit, e.R(row + 0x114), (uint32_t)cls, a3);
    uint32_t pq[2] = {0u - (uint32_t)m * ab[0], 0u - (uint32_t)m * ab[1]};
    e.H(0xc2f8, (uint32_t)(uintptr_t)&pq[0], (uint32_t)(uintptr_t)&pq[1]);
    uint32_t s[2] = {ab[1], ab[0]};   // [0] = S1 = B, [1] = S2 = A
    e.H(0xc2f8, (uint32_t)(uintptr_t)&s[1], (uint32_t)(uintptr_t)&s[0]);
    e.H(0xc7e30, pq[0], 0, 0, 0, 0, o, n * (uint32_t)m, pq[1]);
    uint32_t q = e.R(o + 0x54);
    if (q) {
        uint32_t w = e.R(q + 0x30);
        if (w) {
            e.W(w + 0x10, s[1] << 11);
            e.W(w + 0x14, s[0] << 11);
            e.W(w + 0x18, (0u - n) << 11);
        }
    }
    e.H(0xc7fb0, o);
    return 1;
}
