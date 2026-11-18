// D2Client+0xAA240: slot hit test over nine live rectangles; EBX = x, EDI = y. Returns slot 0..8 or -1.
// Rectangle i: dword x0 at +0 and y0 at +4 of the 0x18-byte record at D2Client+0xDE6E8 + 0x18*i.
// Live flag i: byte at D2Client+0xFCD90 + 5*i. Strict bounds: x0 < x < x0+0x118, y0 < y < y0+0x1E.
#include <cstdint>
#include <emscripten.h>

struct D2cRectRec { int32_t x0; int32_t y0; uint8_t pad[0x10]; };
static_assert(sizeof(D2cRectRec) == 0x18, "rect record stride");

extern "C" {
// 1.13c D2Client+0xAA240
EMSCRIPTEN_KEEPALIVE int32_t d2c_aa240(const D2cRectRec* rects, const uint8_t* flags, int32_t x, int32_t y) {
    for (int32_t i = 0; i < 9; ++i) {
        if (flags[5 * i] == 0) continue;
        if (x <= rects[i].x0 || x >= rects[i].x0 + 0x118) continue;
        if (y <= rects[i].y0 || y >= rects[i].y0 + 0x1E) continue;
        return i;
    }
    return -1;
}
}
