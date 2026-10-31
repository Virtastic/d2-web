// 1.13c D2CMP+0xA710 (ECX = 256 {H,S,L} doubles, EAX = 256 4-byte RGB out): HSL -> RGB.
#include "d2cmp_palstate.h"
static uint8_t ftolByte(double v) { return (uint8_t)(int64_t)v; }  // _ftol2 (0x17C10): truncate, low byte
// 1.13c D2CMP+0x0A710
// S == 0: all three = trunc(L*255). Otherwise m2 = L <= 0.5 ? (S+1)*L : (L+S) - L*S, m1 = 2L - m2,
// R/G/B = trunc(0xA1B0(m1, m2, H+120 / H / H-120) * 255). Byte 3 of each output entry is untouched.
extern "C" void d2cmp_a710(const double* hsl, uint8_t* rgb) {
    for (int32_t i = 0; i < 0x100; ++i, hsl += 3, rgb += 4) {
        double h = hsl[0], s = hsl[1], l = hsl[2];
        uint8_t r, g, b;
        if (s == 0.0) {
            r = g = b = ftolByte(l * 255.0);
        } else {
            double m2 = l <= 0.5 ? (s + 1.0) * l : (l + s) - l * s;
            double m1 = (l + l) - m2;
            r = ftolByte(d2cmp_a1b0(m1, m2, h + 120.0) * 255.0);
            g = ftolByte(d2cmp_a1b0(m1, m2, h) * 255.0);
            b = ftolByte(d2cmp_a1b0(m1, m2, h - 120.0) * 255.0);
        }
        rgb[0] = r; rgb[1] = g; rgb[2] = b;
    }
}
