// 1.13c D2CMP+0xA710 (ECX = 256 {H,S,L} doubles, EAX = 256 4-byte RGB out): HSL -> RGB.
#include "d2cmp_palstate.h"
#include <cfloat>
long double d2cmp_a1b0_st0(double m1, double m2, double hue);   // 0xA1B0 as left in ST0
static inline long double r64(long double x) {
#if LDBL_MANT_DIG > 64
    const long double c = x * (long double)(1ull << (LDBL_MANT_DIG - 64)) + x;   // Veltkamp split to 64 mantissa bits
    return c - (c - x);
#else
    return x;
#endif
}
// _ftol2 (0x17C10): truncate the x87 value, low byte. Retail multiplies by 255 in extended precision (exact for a double), so the product is not rounded to a double.
static uint8_t ftolByte(long double v) { return (uint8_t)(int64_t)v; }
// 1.13c D2CMP+0x0A710 -- retail's x87 runs at 64-bit mantissa: m2 is built in extended and stored as a double (the extended m2 also feeds m1),
// m1 = 2L - m2(extended) is stored as a double, H+/-120 are stored as doubles, the 0xA1B0 result is multiplied by 255 unrounded.
// S == 0: all three = trunc(L*255). Otherwise m2 = L <= 0.5 ? (S+1)*L : (L+S) - L*S, m1 = 2L - m2,
// R/G/B = trunc(0xA1B0(m1, m2, H+120 / H / H-120) * 255). Byte 3 of each output entry is untouched.
extern "C" void d2cmp_a710(const double* hsl, uint8_t* rgb) {
    for (int32_t i = 0; i < 0x100; ++i, hsl += 3, rgb += 4) {
        double h = hsl[0], s = hsl[1], l = hsl[2];
        uint8_t r, g, b;
        if (s == 0.0) {
            r = g = b = ftolByte((long double)l * 255.0L);
        } else {
            long double m2e = l <= 0.5 ? r64(r64((long double)s + 1.0L) * l) : r64(r64((long double)l + s) - r64((long double)l * s));
            const double m2 = (double)m2e;
            const double m1 = (double)r64(((long double)l + l) - m2e);
            const double hp = (double)r64((long double)h + 120.0L), hm = (double)r64((long double)h - 120.0L);
            r = ftolByte(r64(d2cmp_a1b0_st0(m1, m2, hp) * 255.0L));
            g = ftolByte(r64(d2cmp_a1b0_st0(m1, m2, h) * 255.0L));
            b = ftolByte(r64(d2cmp_a1b0_st0(m1, m2, hm) * 255.0L));
        }
        rgb[0] = r; rgb[1] = g; rgb[2] = b;
    }
}
