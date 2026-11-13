// 1.13c D2Common+0x184C0 -- stdcall (p0, p1, maxLen, &dir) ret 0x10, EDI = out buffer of (x, y) 16-bit pairs.
// Integer line walk from p0 toward p1 (the start itself is not emitted). *dir: 1/3 for an x-major or horizontal run
// (sx > 0 / else), 2/0 for a y-major or vertical one (sy > 0 / else). More than maxLen - 1 points: out[0].x = 0, result 0.
// Written from the disassembly (the unaligned load of p0's high word is the y start; its junk upper half never reaches a store).
#include "d2common_18c40.h"

// 1.13c D2Common+0x184C0
extern "C" int32_t D2Common_184C0_LineToPoints(uint32_t p0, uint32_t p1, int32_t maxLen, int32_t* dirOut, uint16_t* out)
{
    const int32_t lim = maxLen - 1;
    const uint16_t x0 = uint16_t(p0), y0 = uint16_t(p0 >> 16);
    int32_t dx = int32_t(uint16_t(p1)) - int32_t(x0);
    int32_t dy = int32_t(p1 >> 16) - int32_t(y0);
    int32_t sx = 1, sy = 1;
    int32_t cnt = 0;
    if (dx < 0) { dx = -dx; sx = -1; }
    if (dy < 0) { dy = -dy; sy = -1; }
    *dirOut = 0;
    auto put = [&](uint16_t x, uint16_t y) { out[cnt * 2] = x; out[cnt * 2 + 1] = y; ++cnt; };
    auto fail = [&]() { out[0] = 0; return 0; };
    auto done = [&]() { out[cnt * 2] = 0; return cnt; };
    uint16_t x = x0, y = y0;
    if (dx == 0) {
        if (dy <= 0) return done();
        *dirOut = sy > 0 ? 2 : 0;
        if (dy > lim) return fail();
        for (int32_t k = dy; k; --k) { y = uint16_t(y + sy); put(x, y); }
        return done();
    }
    if (dy == 0) {
        *dirOut = sx > 0 ? 1 : 3;
        if (dx > lim) return fail();
        for (int32_t k = dx; k; --k) { x = uint16_t(x + sx); put(x, y); }
        return done();
    }
    int32_t err = 0;
    if (dx < dy) {
        *dirOut = sy > 0 ? 2 : 0;
        if (dy > lim) return fail();
        for (int32_t k = dy; k; --k) {
            y = uint16_t(y + sy);
            err += dx;
            if (err >= dy) { err -= dy; x = uint16_t(x + sx); }
            put(x, y);
        }
        return done();
    }
    *dirOut = sx > 0 ? 1 : 3;
    if (dx > lim) return fail();
    for (int32_t k = dx; k; --k) {
        err += dy;
        x = uint16_t(x + sx);
        if (err >= dx) { err -= dx; y = uint16_t(y + sy); }
        put(x, y);
    }
    return done();
}
