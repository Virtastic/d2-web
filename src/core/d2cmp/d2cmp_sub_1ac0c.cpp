#include <cstdint>
#include <cstring>

// Host-bound: 1A9D6 = 80-bit multiply (x *= y); tables at RVA 0x342E0 (positive) / 0x34440 (negative), 12-byte entries.
void (*g_d2cmp_mul80)(int* x, int* y) = nullptr;
const uint8_t* g_d2cmp_tenpow_pos = nullptr;
const uint8_t* g_d2cmp_tenpow_neg = nullptr;

// 1.13c D2CMP+0x1AC0C  (CRT _multtenpow12: x *= 10^n; n in octal digits, one table level per 3 bits)
void d2cmp_multtenpow12(int* x, uint32_t n, int keep)
{
    const uint8_t* t = g_d2cmp_tenpow_pos;
    if (n == 0) return;
    if ((int32_t)n < 0) { n = -(int32_t)n; t = g_d2cmp_tenpow_neg; }
    if (keep == 0) *(uint16_t*)x = 0;
    while (n != 0) {
        uint32_t d = n & 7;
        n = (int32_t)n >> 3;
        t += 0x54;
        if (d) {
            const uint8_t* e = t + d * 12;
            uint8_t tmp[12];
            if (*(const uint16_t*)e > 0x7fff) {
                memcpy(tmp, e, 12);
                uint32_t v; memcpy(&v, tmp + 2, 4); v--; memcpy(tmp + 2, &v, 4);
                e = tmp;
            }
            g_d2cmp_mul80(x, (int*)e);
        }
    }
}
