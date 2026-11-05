// 1.13c Storm+0x3BB50: fills the 20 x 10 table of doubles used by the number
// formatter, row e (exponents -1 .. -20), column k (digits 0 .. 9): (1 / 10^e, integer division) * k. 10^e is a
// 32-bit product that wraps, and the integer quotient of 1 by it is 0 for every row, so each entry is +0.0.
#include <cstdint>
#include <cstring>

static double g_tab3bb50[20 * 10];
extern "C" double* Storm_3BB50_Table() { return g_tab3bb50; }

extern "C" void Storm_3BB50() {
    double* d = g_tab3bb50;
    for (int32_t e = -1; e > -0x15; e--)
        for (int32_t k = 0; k < 10; k++) {
            int32_t b = 10, p = 1;
            for (uint32_t u = e < 0 ? (uint32_t)-e : (uint32_t)e;;) {
                if (u & 1) p = (int32_t)((uint32_t)p * (uint32_t)b);
                u >>= 1;
                if (!u) break;
                b = (int32_t)((uint32_t)b * (uint32_t)b);
            }
            int32_t q = e < 0 ? 1 / p : p;
            *d++ = (double)q * (double)k;
        }
}
