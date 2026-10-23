#include <cstdint>

// 1.13c D2Gfx+0x0B920 (ordinal 10012): stdcall(x, y). Renderer mode (+0x11258) 4..7 and +0x10BE0 set -> 1 iff |x - *w1124c| <= 0x320000
// and |y - *w11248| <= 0x320000 (32-bit wrap: abs(INT_MIN) stays negative, so it passes). Else retail logs (Fog 10265/10024) and _exit(-1), not modelled.
extern "C" uint32_t d2gfx_sub_b920(int32_t x, int32_t y, int32_t w1124c, int32_t w11248) {
    int32_t a = (int32_t)((uint32_t)x - (uint32_t)w1124c); if (a < 0) a = (int32_t)(0u - (uint32_t)a);
    if (0x320000 < a) return 0;
    int32_t b = (int32_t)((uint32_t)y - (uint32_t)w11248); if (b < 0) b = (int32_t)(0u - (uint32_t)b);
    return b < 0x320001;
}
