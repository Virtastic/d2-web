// src/core/d2ddraw/d2ddraw_9340.cpp -- 1.13c D2DDraw.dll debug-font glyph draw: stamp one 8x16 glyph of the GDI-rasterised
// Courier font into the 8-bit frame buffer as 0xFF and return the glyph's advance. Decompile-derived, checked against the
// disassembly. The module globals arrive as pointers.
#include <cstdint>

struct D2DDrawGlyphEnv {
    const uint32_t* pitch;        // DAT 0x101C8: the frame buffer pitch (the display width)
    uint8_t* const* screen;       // DAT 0xF8A8: the frame buffer
    const uint8_t* const* glyphs; // DAT 0x101F4: 96 glyphs of 8x16 bytes
    const uint32_t* offsets;      // DAT 0xF380: 96 byte offsets into the glyphs
    const int32_t* widths;        // DAT 0xF500: 96 advances (GetCharWidthA); retail indexes it unchecked, see below
};

// 1.13c D2DDraw+0x9340 (ECX = x, AL = c, [esp+4] = y; ret 4). x > 640 returns 0. x < 0 returns widths[(int8)c] without the
// 0x20 bias (retail's own indexing: a negative or high char reads outside the table). A signed char below ' ' returns
// widths[0]. Otherwise every non-zero byte of glyph c-0x20 sets the frame-buffer byte at (x + col, y + row) to 0xFF, no
// clipping, and widths[c-0x20] is returned.
extern "C" int32_t d2ddraw_draw_glyph_9340(const D2DDrawGlyphEnv* e, int32_t x, int8_t c, int32_t y) {
    if (x > 0x280) return 0;
    if (x < 0) return e->widths[c];
    if (c < 0x20) return e->widths[0];
    int32_t i = (int8_t)(c - 0x20);
    uint32_t pitch = *e->pitch;
    const uint8_t* src = *e->glyphs + e->offsets[i];
    uint8_t* dst = *e->screen + pitch * (uint32_t)y + (uint32_t)x;
    for (int row = 0; row < 16; ++row, dst += pitch)
        for (int col = 0; col < 8; ++col, ++src)
            if (*src) dst[col] = 0xFF;
    return e->widths[i];
}
