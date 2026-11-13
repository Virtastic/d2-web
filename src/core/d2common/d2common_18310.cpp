// 1.13c D2Common+0x18310 -- stdcall (start, out, n) ret 0xC, EDX = packed points (x low word, y high word).
// Keeps a point when the step to the next differs from the step into it, with one extra rule: after a change in both
// axes at once the step is marked -2 (never equal) and the point is dropped; the last point is always kept.
#include "d2common_18c40.h"

// 1.13c D2Common+0x18310
extern "C" int32_t D2Common_18310_ThinPoints(const uint32_t* pts, uint32_t start, uint32_t* out, int32_t n)
{
    if (n < 2) {
        if (n == 1) { out[0] = pts[0]; return 1; }
        return 0;
    }
    int32_t pdx = int32_t(uint16_t(pts[0])) - int32_t(uint16_t(start));
    int32_t pdy = int32_t(pts[0] >> 16 & 0xFFFF) - int32_t(start >> 16);
    int32_t cnt = 0, k = 0;
    for (int32_t i = 0; i < n - 1; ++i) {
        const int32_t ddx = int32_t(uint16_t(pts[i + 1])) - int32_t(uint16_t(pts[i]));
        const int32_t ddy = int32_t(pts[i + 1] >> 16 & 0xFFFF) - int32_t(pts[i] >> 16 & 0xFFFF);
        int32_t ndx = ddx;
        if (ddx == pdx && ddy == pdy) {
            ++k;
        } else if (k > 0 || pdx == ddx || pdy == ddy) {
            out[cnt++] = pts[i];
            k = 0;
        } else {
            k = 1;
            ndx = -2;
        }
        pdx = ndx;
        pdy = ddy;
    }
    out[cnt++] = pts[n - 1];
    return cnt;
}
