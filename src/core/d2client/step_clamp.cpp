// 1.13c D2Client+0x186A0 -- clamp a move step toward (step[3],step[4]) to a flag-selected length.
// Unit types 2,4,5 only: pPath = unit[0xb]; other types call D2Common ordinals and type 0 uses globals 6FBC97F8/6FBD34C4.
#include <emscripten.h>
#include <cstdint>
extern "C" EMSCRIPTEN_KEEPALIVE int d2_client_186a0(const int32_t* unit, int32_t* step) {
    const int32_t* path = (const int32_t*)(uintptr_t)(uint32_t)unit[0xb];
    int32_t px = path[3], py = path[4];
    int32_t dx = step[3] - px, dy = step[4] - py;
    int32_t ax = dx < 0 ? -dx : dx, ay = dy < 0 ? -dy : dy;
    if (ax >= 0x100 || ay >= 0x100) return 0;
    if (ax < 1 && ay < 1) return 0;
    // 6FB587B0: octagonal distance
    int32_t hi = ax >= ay ? ax : ay, lo = ax >= ay ? ay : ax;
    int32_t d = (int32_t)((uint32_t)(hi * 0x3d7 + lo * 0x197) >> 10);
    if (d < 2) d = 1;
    uint32_t f = (uint32_t)step[0];
    int32_t lim = 3;
    if ((f & 4) && (f & 0x40)) lim = 5;
    else if (f & 8) lim = ((f & 0xff) >> 6 & 1) | 4;
    else if (f & 0x10) lim = 0;
    if (d < lim) {  // x87 ratio, truncating convert (6FB7ACC4)
        long double q = (long double)lim / (long double)d;
        dx = (int32_t)((long double)dx * q);
        dy = (int32_t)((long double)dy * q);
    }
    step[3] = px + dx;
    step[4] = py + dy;
    return 1;
}
