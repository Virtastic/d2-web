// src/core/d2win/d2win_12d10.cpp -- 1.13c D2Win.dll+0x12D10, draw a tooltip box (frame clamped to the screen) and its text
// Decompile-derived from the retail bytes and the cached Ghidra decompile, resolved against the disassembly (the decompile
// loses every argument register and the stack-slot reuse by the screen-size call). Imported callees and D2Win state are
// seams on the Env struct, passed last, so the retail argument order is preserved.
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2Win12D10Env {
    const uint32_t* fontIndex;                     // 0x6F8FFD34
    const uint32_t* fontTable;                     // 0x6F901518, 5 dwords per entry
    int32_t (*textHeight)(const uint16_t* text);   // D2Win+0x11E80 (text in ESI)
    void (*screenSize)(int32_t* w, int32_t* h);    // D2Gfx #10080
    void (*fillRect)(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t color, int32_t mode);  // D2Gfx #10014
    // D2Win+0x12940 (x in EAX; text, y, width, colour, centred on the stack).
    void (*drawLines)(int32_t x, const uint16_t* text, int32_t y, int32_t width, int32_t color, int32_t centered);
};

// Entry (text on the stack; x, y, width, fillColor, fillMode, textColor follow; ret 0x1c). The box is
// `width` wide from x (pulled left so it ends at the screen's right edge); its bottom is y + 2 capped at screenH - 31
// and doubled when that leaves less than the text height. The frame is filled from 4 left of x, its top at height*4/10
// minus the text height above the bottom; the text is drawn at the bottom - 2.
// 1.13c D2Win+0x12D10
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_12D10(const uint16_t* text, int32_t x, int32_t y, int32_t width,
                                                    int32_t fillColor, int32_t fillMode, int32_t textColor,
                                                    const D2Win12D10Env* e) {
    const int32_t r = e->textHeight(text);
    const int32_t h = (int32_t)font_height(e->fontTable, *e->fontIndex);
    const int32_t hh = (h * 4) / 10;
    int32_t sw = 0, sh = 0;
    e->screenSize(&sw, &sh);
    int32_t left = x;
    int32_t b = y + 2;
    if (x + width > sw) left = sw - width;
    if (b > sh - 0x1f) b = sh - 0x1f;
    if (b - r < 0) b *= 2;
    e->fillRect(left - 4, hh - r + b, width + (left - 4), b, fillColor, fillMode);
    e->drawLines(left, text, b - 2, 0, textColor, 0);
}
