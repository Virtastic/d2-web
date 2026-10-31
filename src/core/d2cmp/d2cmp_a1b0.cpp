// 1.13c D2CMP+0xA1B0 (stdcall, three doubles): the HSL "hue to channel" helper.
// x87 at the Windows default 53-bit precision control, so every step is a double operation.
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x0A1B0
// hue > 360 loses 360, hue < 0 gains 360 (once each). Below 60 rises linearly m1 -> m2, below 180
// holds m2, below 240 falls linearly back, else m1. The slope multiplies by the constant 1/60.
extern "C" double d2cmp_a1b0(double m1, double m2, double hue) {
    const double kInv60 = 1.0 / 60.0;  // 0x6FE300C8
    if (hue > 360.0) hue -= 360.0;
    if (hue < 0.0) hue += 360.0;
    if (hue < 60.0) return hue * (m2 - m1) * kInv60 + m1;
    if (hue < 180.0) return m2;
    if (hue < 240.0) return (m2 - m1) * (240.0 - hue) * kInv60 + m1;
    return m1;
}
