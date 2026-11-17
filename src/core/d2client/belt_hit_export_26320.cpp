// The browser's belt popup asks the retail belt hit test (belt_hit_26320.cpp) instead of its own rectangles.
// The caller holds the worn belt's box rects; the expanded test reads only the first box of the top row
// (entry count-4: left, top) and the fourth box (right, bottom), so those four numbers stand in for the
// Ordinal_10991 fill.
#include <cstdint>
#include <cstring>
#if __has_include(<emscripten.h>)
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

namespace d2client_belt_hit {
struct Env { int32_t expanded, width, height; void (*fill)(uint32_t*); };
uint32_t sub_26320(const Env&, const uint8_t*);
}

namespace {
struct Rects { uint32_t count, x0, y0, x1, y1; };
Rects g_rects;
void fill_rects(uint32_t* b) {
    const int k = 2 + 4 * (int(g_rects.count & 0xff) - 4);
    b[1] = g_rects.count;
    if (k >= 0 && k + 2 < 66) { b[k] = g_rects.x0; b[k + 2] = g_rects.y0; }
    b[15] = g_rects.x1;
    b[17] = g_rects.y1;
}
}

extern "C" {
// 1.13c D2Client+0x26320: 1 when the point (x, y) is inside the belt popup's mouse box.
EMSCRIPTEN_KEEPALIVE int32_t d2_client_belt_hit(int32_t expanded, int32_t width, int32_t height, int32_t x, int32_t y,
                                                int32_t count, int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
    g_rects = {(uint32_t)count, (uint32_t)x0, (uint32_t)y0, (uint32_t)x1, (uint32_t)y1};
    d2client_belt_hit::Env e{expanded, width, height, fill_rects};
    uint8_t ev[16] = {0};
    const uint16_t ux = (uint16_t)x, uy = (uint16_t)y;
    std::memcpy(ev + 0xc, &ux, 2);
    std::memcpy(ev + 0xe, &uy, 2);
    return (int32_t)d2client_belt_hit::sub_26320(e, ev);
}
}
