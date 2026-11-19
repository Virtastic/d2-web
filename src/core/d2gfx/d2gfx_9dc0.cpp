// 1.13c D2Gfx+0x9dc0: fills t[32][32][32] (D2Gfx+0x15450 in retail).
// The probe src/core/fn/d2gfx_9dc0.cpp drives this body.
#include <cstdint>

// 1.13c D2Gfx+0x9dc0
extern "C" void d2gfx_9dc0(uint8_t* t) {
    int32_t base = 0;
    for (int a = 0; a < 32; a++, base -= 0x1000000)
        for (int b = 0, c = base; b < 32; b++, c += 0x1000000) {
            int32_t v = a << 24;
            for (int k = 0; k < 32; k++) {
                if (v < 1) v = 0; else if (v > 0x1effffff) v = 0x1f000000;
                t[(a * 32 + b) * 32 + k] = (uint8_t)((uint32_t)v >> 24);
                v += c >> 5;
            }
        }
}
