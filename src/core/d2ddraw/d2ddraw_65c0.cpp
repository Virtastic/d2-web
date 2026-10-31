// 1.13c D2DDraw+0x065c0 -- byte Bresenham line on the 8-bit back buffer. Decompile-derived.
#include <cstdint>

struct D2DDrawSurf8 { uint8_t* buf; int32_t width; int32_t height; };  // +0xF8A8, +0x101C8, +0x101D8

// 1.13c D2DDraw+0x065c0
// Plots (x0,y0) if on the surface, then steps max(|dx|,|dy|) times along the major axis, carrying the
// minor axis when the error exceeds the major delta (strict >). Each point is clipped individually.
extern "C" void d2ddraw_line_bresenham(const D2DDrawSurf8* s, int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                                       uint8_t color) {
    const int32_t w = s->width;
    int32_t dx = x1 - x0, dy = y1 - y0, sx, sy = -1;
    if (dx < 0) { sx = -1; dx = -dx; } else sx = 1;
    if (dy < 0) dy = -dy; else sy = 1;
    if (x0 > -1 && x0 < s->width && y0 > -1 && y0 < s->height) s->buf[w * y0 + x0] = color;
    int32_t err = 0;
    if (dx < dy) {
        if (dy <= 0) return;
        int32_t row = w * y0;
        for (int32_t n = dy; n; --n) {
            y0 += sy; err += dx; row += w * sy;
            if (dy < err) { x0 += sx; err -= dy; }
            if (x0 > -1 && x0 < s->width && y0 > -1 && y0 < s->height) s->buf[row + x0] = color;
        }
    } else if (dx > 0) {
        int32_t row = w * y0;
        for (int32_t n = dx; n; --n) {
            x0 += sx; err += dy;
            if (dx < err) { y0 += sy; row += w * sy; err -= dx; }
            if (x0 > -1 && x0 < s->width && y0 > -1 && y0 < s->height) s->buf[row + x0] = color;
        }
    }
}
