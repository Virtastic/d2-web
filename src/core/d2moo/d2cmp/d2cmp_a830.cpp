// 1.13c D2CMP+0xA830 (ECX = 256 {H,S,L} doubles out, EDX = 256 4-byte RGB in): RGB -> HSL.
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x0A830
// Channels scale by the constant 1/255. L = (max+min)*0.5, fatal 0x3F outside [0,1]. max == min:
// H = S = 0. Else S = delta/sum (L <= 0.5) or delta/(2-sum), fatal 0x4D outside [0,1]; H = 60 *
// ((g-b)/delta | (b-r)/delta + 2 | (r-g)/delta + 4) for max == r | g | otherwise, +360 if negative,
// fatal 0x5C unless 0 <= H < 360. (The FDIV-erratum branch is not taken: 0x6FF13BDC is 0.)
extern "C" void d2cmp_a830(const D2CmpPalEnv* env, double* hsl, const uint8_t* pal) {
    const double kInv255 = 1.0 / 255.0;  // 0x6FE30118
    for (int32_t i = 0; i < 0x100; ++i, pal += 4, hsl += 3) {
        double r = (double)pal[0] * kInv255, g = (double)pal[1] * kInv255, b = (double)pal[2] * kInv255;
        double mx = g <= b ? b : g;
        if (r > mx) mx = r;
        double mn = g < b ? g : b;
        if (r < mn) mn = r;
        double sum = mx + mn, l = sum * 0.5;
        if (l < 0.0 || l > 1.0) env->fatal(0x3f);
        double h, s;
        if (mx == mn) {
            h = 0.0; s = 0.0;
        } else {
            double d = mx - mn;
            s = l <= 0.5 ? d / sum : d / (2.0 - sum);
            if (s < 0.0 || s > 1.0) env->fatal(0x4d);
            if (r == mx) h = (g - b) / d;
            else if (g == mx) h = (b - r) / d + 2.0;
            else h = (r - g) / d + 4.0;
            h *= 60.0;
            if (h < 0.0) h += 360.0;
            if (h < 0.0 || h >= 360.0) env->fatal(0x5c);
        }
        hsl[0] = h; hsl[1] = s; hsl[2] = l;
    }
}
