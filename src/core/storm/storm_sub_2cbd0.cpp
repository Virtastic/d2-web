// 1.13c Storm+0x2CBD0 -- bignum: this = src * (64-bit scalar), little-endian dword vectors.
#include <cstdint>
struct S2CBigVec { uint32_t cap, size; uint32_t* data; uint32_t grow, off; };  // retail dwords 0..4
// seams: Storm+0x3C80 (set capacity), Storm+0x3C40 (default grow); unmapped here
void Seam3C80_2CBD0(S2CBigVec*, uint32_t) {}
uint32_t Seam3C40_2CBD0(S2CBigVec*) { return 1; }

void Storm_2CBD0(S2CBigVec* dst, const S2CBigVec* src, uint32_t mlo, uint32_t mhi) {
    uint64_t m = ((uint64_t)mhi << 32) | mlo;
    uint32_t carry = 0;
    int i = 0;
    for (; carry != 0 || src->off + i < src->size; i++) {
        uint32_t idx = src->off + i, v = idx < src->size ? src->data[idx] : 0;
        uint64_t p = (uint64_t)v * m;  // __allmul(v,0,lo,hi): low 64 bits
        uint32_t pos = dst->off + i;
        if (dst->size <= pos) {
            uint32_t n = pos + 1;
            if (dst->cap < n) {
                uint32_t g = dst->grow;
                if (g == 0) {
                    if (n < 0x40) {
                        g = n;
                        for (uint32_t t = pos & n; t; t = (t - 1) & t) g = t;
                        if (g == 0) g = 1;
                    } else dst->grow = g = 0x40;
                }
                uint32_t c = n;
                if (n % g) c = (g - n % g) + n;
                Seam3C80_2CBD0(dst, c);
            }
            for (uint32_t k = dst->size; k < n; k++) dst->data[k] = 0;
            dst->size = n;
        }
        uint64_t s = p + carry;
        carry = (uint32_t)(s >> 32);
        dst->data[dst->off + i] = (uint32_t)s;
    }
    uint32_t e = dst->off + i;
    if (dst->size < e && dst->cap < e) {
        uint32_t g = dst->grow ? dst->grow : Seam3C40_2CBD0(dst);
        uint32_t c = e;
        if (e % g) c = (g - e % g) + e;
        Seam3C80_2CBD0(dst, c);
    }
    dst->size = e;
}
