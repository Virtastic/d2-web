// 1.13c D2CMP+0xAAD0 (ECX = 256 {H,S,L} doubles, EDI = shift block): saturation-boost table at +0x3400.
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x0AAD0
// Every entry with L (the third double, edx = hsl + 0x10) != 0 gets L += 0.2f (the float constant at 0x6FE30120 widened,
// 0.20000000298...) clamped to 1, written back into the caller's HSL array; the sum is formed in extended precision, stored as
// a double, and the clamp compares the unrounded sum. 0xA710 converts the array and each result
// is matched against the block's 256 entries with 0x9D30.
#include <cfloat>
static inline long double r64(long double x) {
#if LDBL_MANT_DIG > 64
    const long double c = x * (long double)(1ull << (LDBL_MANT_DIG - 64)) + x;
    return c - (c - x);
#else
    return x;
#endif
}
extern "C" void d2cmp_aad0(double* hsl, uint8_t* block) {
    const double kBoost = (double)0.2f;
    for (int32_t i = 0; i < 0x100; ++i) {
        double& l = hsl[i * 3 + 2];
        if (l != 0.0) {
            const long double sum = r64((long double)l + kBoost);
            l = (double)sum;
            if (sum > 1.0L) l = 1.0;
        }
    }
    uint8_t rgb[0x400];
    d2cmp_a710(hsl, rgb);
    for (uint32_t i = 0; i < 0x100; ++i)
        block[0x3400 + i] = (uint8_t)d2cmp_sub_9d30(block, 0x100, rgb[i * 4], rgb[i * 4 + 1], rgb[i * 4 + 2]);
}
