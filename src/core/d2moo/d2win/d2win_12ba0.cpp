// src/core/d2win/d2win_12ba0.cpp -- 1.13c D2Win.dll+0x12BA0, draw a hint box: framed, clamped to the screen, optionally part-filled
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile or, where Ghidra defines no function, the
// disassembly. Imported callees and D2Win state are seams on the Env struct, passed last, so the
// retail argument order is preserved. Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win/
// (control type numbering and header layout only; D2WinFont.cpp for the L"c" colour code).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2Win12BA0Env {
    const uint32_t* fontIndex;            // 0x6F8FFD34
    const uint32_t* fontTable;            // 0x6F901518, 5 dwords per entry
    uint8_t* fillColor;                   // 0x6F9A9D10, cached palette index (0 = not yet looked up)
    const uint8_t* palette;               // 0x6F922338
    int32_t (*textWidth)(const uint16_t* text);    // D2Win+0x11FB0 (EAX)
    int32_t (*textHeight)(void);                   // D2Win+0x11E80
    void (*screenSize)(int32_t* w, int32_t* h);    // D2Gfx #10080
    uint8_t (*nearestColor)(const uint8_t* palette, int32_t n, int32_t r, int32_t g, int32_t b);  // D2CMP #10049
    void (*fillRect)(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color, int32_t mode);  // D2Gfx #10014
    // D2Win+0x12940 (x in EAX; text, y, width, colour, centred on the stack).
    void (*drawLines)(int32_t x, const uint16_t* text, int32_t y, int32_t width, int32_t color, int32_t centered);
};

// 1.13c D2Win+0x12BA0 (ordinal 10107; text in ECX, x in EDX; y, colour, centre, fill on the stack).
// Box width = text width + 8, height = textHeight(); bottom = y + 2. centre == 1 shifts x left by
// width/2. x is clamped to 0 .. screenW - width, bottom to height .. screenH - 31. fill != 0 draws
// the first (width * fill) >> 7 pixels in the cached nearest (0, 0x50, 0) mode 1 and the rest in
// colour 0 mode 2; fill == 0 draws the whole box in colour 0 mode 2. The text goes on top,
// centred, at bottom - height*3/10. Retail pushes the colour dword with leftover upper bytes;
// the palette index is the low byte passed here.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_12BA0(const uint16_t* text, int32_t x, int32_t y, int32_t color,
                                                    int32_t center, int32_t fill, const D2Win12BA0Env* env) {
    if (u16len(text) == 0) return;
    int32_t w = env->textWidth(text) + 8;
    int32_t h = env->textHeight();
    if (center == 1) x -= w >> 1;
    int32_t b = y + 2;
    int32_t sw = 0, sh = 0;
    env->screenSize(&sw, &sh);
    if (x <= 0) x = 0;
    if (x >= sw - w) x = sw - w;
    if (b <= h) b = h;
    if (b >= sh - 0x1F) b = sh - 0x1F;
    if (fill) {
        uint8_t c = *env->fillColor;
        if (!c) { c = env->nearestColor(env->palette, 0x100, 0x50, 0, 0); *env->fillColor = c; }
        int32_t part = (w * fill) >> 7;
        env->fillRect(x, b - h, part + x, b, c, 1);
        if (w - part != 0) env->fillRect(part + x, b - h, (w - part) + (part + x), b, 0, 2);
    } else {
        env->fillRect(x, b - h, x + w, b, 0, 2);
    }
    int32_t hgt = (int32_t)font_height(env->fontTable, *env->fontIndex);
    env->drawLines(x, text, b - (hgt * 3) / 10, w, color, 1);
}
