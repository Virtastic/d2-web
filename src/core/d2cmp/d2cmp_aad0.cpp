// 1.13c D2CMP+0xAAD0 (ECX = 256 {H,S,L} doubles, EDI = shift block): saturation-boost table at +0x3400.
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x0AAD0
// Every entry with S != 0 gets S += 0.2f (the float constant at 0x6FE30120 widened, 0.20000000298...)
// clamped to 1, written back into the caller's HSL array; 0xA710 converts the array and each result
// is matched against the block's 256 entries with 0x9D30.
extern "C" void d2cmp_aad0(double* hsl, uint8_t* block) {
    const double kBoost = (double)0.2f;
    for (int32_t i = 0; i < 0x100; ++i) {
        double& s = hsl[i * 3 + 1];
        if (s != 0.0) {
            s += kBoost;
            if (s > 1.0) s = 1.0;
        }
    }
    uint8_t rgb[0x400];
    d2cmp_a710(hsl, rgb);
    for (uint32_t i = 0; i < 0x100; ++i)
        block[0x3400 + i] = (uint8_t)d2cmp_sub_9d30(block, 0x100, rgb[i * 4], rgb[i * 4 + 1], rgb[i * 4 + 2]);
}
