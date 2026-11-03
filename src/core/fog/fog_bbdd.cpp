// 1.13c Fog+0x0BBDD __ld12cvt (cdecl, 3 args: ld, out, params). Decompile-derived.
// Callees: __RoundMan 0xBABC (its IncMan/ZeroTail 0xBA6F/0xBA3D), __ShrMan 0xBB62, __CopyMan 0xBB2E, __IsZeroMan 0xBB49,
// ___addl 0xC0FC (returns the carry). params = DAT_6ff7ed20 (double) / DAT_6ff7ed38 (float): {maxExp, minExp, roundBits,
// manBits, totalBits, bias}. Linkable (extern "C") so the FN probe can call it; the anonymous-namespace copy is ld12cvt_z12.
#include <cstdint>
#include <cstring>

namespace {

uint32_t addl(uint32_t a, uint32_t b, uint32_t* out) {          // ___addl 0xC0FC
    uint32_t s = a + b;
    *out = s;
    return (s < a || s < b) ? 1u : 0u;
}

int32_t zeroTail(const uint32_t* m, int p) {                     // __ZeroTail 0xBA3D
    int idx = p / 32;
    uint32_t u = m[idx] & ~(0xFFFFFFFFu << (31 - (p % 32)));
    for (;;) {
        if (u != 0) return 0;
        if (++idx > 2) break;
        u = m[idx];
    }
    return 1;
}

uint32_t incMan(uint32_t* m, int p) {                            // __IncMan 0xBA6F: returns the last carry
    int idx = p / 32;
    uint32_t c = addl(m[idx], 1u << (31 - (p % 32)), &m[idx]);
    for (idx--; idx >= 0; idx--) {
        if (c == 0) return c;
        c = addl(m[idx], 1u, &m[idx]);
    }
    return c;
}

int32_t roundMan(uint32_t* m, int p) {                           // __RoundMan 0xBABC
    int32_t ret = 0;
    int idx = p / 32;
    uint32_t sh = 31 - (uint32_t)(p % 32);
    if ((m[idx] & (1u << sh)) != 0 && zeroTail(m, p) == 0) ret = (int32_t)incMan(m, p - 1);
    m[idx] &= 0xFFFFFFFFu << sh;
    for (int i = idx + 1; i < 3; i++) m[i] = 0;
    return ret;
}

void shrMan(uint32_t* m, int n) {                                // __ShrMan 0xBB62
    uint32_t b = (uint32_t)(n % 32) & 31u;
    uint32_t carry = 0;
    for (int i = 0; i < 3; i++) {
        uint32_t u = m[i];
        m[i] = (u >> b) | carry;
        carry = (u & ~(0xFFFFFFFFu << b)) << ((32u - b) & 31u);
    }
    int q = n / 32;
    for (int i = 2; i >= 0; i--) m[i] = (i < q) ? 0u : m[i - q];
}

}  // namespace

extern "C" uint32_t fog_ld12cvt_bbdd(const uint16_t* ld, uint32_t* out, const int32_t* P) {
    const int32_t maxExp = P[0], minExp = P[1], roundBits = P[2], manBits = P[3], totalBits = P[4], bias = P[5];
    uint16_t se = ld[5];
    uint32_t man[3];                                             // local_10, local_c, local_8
    uint32_t save[3];
    memcpy(&man[0], ld + 3, 4);
    memcpy(&man[1], ld + 1, 4);
    man[2] = (uint32_t)ld[0] << 16;
    uint32_t e = se & 0x7fffu;
    int32_t ex = (int32_t)e - 0x3fff;
    uint32_t status;
    if (ex == -0x3fff) {
        ex = 0;
        if (man[0] == 0 && man[1] == 0 && man[2] == 0) { status = 0; goto fin; }
        man[0] = 0; man[1] = 0;
    } else {
        memcpy(save, man, sizeof man);
        if (roundMan(man, roundBits)) ex = (int32_t)e - 0x3ffe;
        if (ex < minExp - roundBits) { man[0] = 0; man[1] = 0; }
        else {
            if (minExp < ex) {
                if (maxExp <= ex) {
                    man[0] = 0x80000000u; man[1] = 0; man[2] = 0;
                    shrMan(man, manBits);
                    ex = bias + maxExp;
                    status = 1; goto fin;
                }
                man[0] &= 0x7fffffffu;
                ex = bias + ex;
                shrMan(man, manBits);
                status = 0; goto fin;
            }
            memcpy(man, save, sizeof man);
            shrMan(man, minExp - ex);
            roundMan(man, roundBits);
            shrMan(man, manBits + 1);
        }
    }
    ex = 0; status = 2;
fin:
    man[0] = ((uint32_t)ex << ((31u - (uint32_t)manBits) & 31u)) |
             ((se & 0x8000u) ? 0x80000000u : 0u) | man[0];
    if (totalBits == 0x40) { out[1] = man[0]; out[0] = man[1]; }
    else if (totalBits == 0x20) out[0] = man[0];
    return status;
}
