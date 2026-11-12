// src/core/d2win/d2win_a0e0.cpp -- 1.13c D2Win.dll+0x0A0E0, list-box (type 0xC) draw.
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll) and the cached Ghidra decompile, resolved against
// the disassembly (the decompile loses EAX = x for the text call). Callees are seams on the Env struct.
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2WinA0E0Env {
    const uint32_t* fontIndex;                                                                      // 0x6F8FFD34
    const uint32_t* fontTable;                                                                      // 0x6F901518, 5 dwords per entry
    void (*setFont)(int32_t index);                                                                 // D2Win+0x12FE0 (ECX)
    void (*fillRect)(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t color, int32_t mode);  // D2Gfx #10014
    void (*toUnicode)(uint16_t* dst, const char* src, int32_t n);                                   // D2Lang win2Unicode (ECX dst, EDX src, n)
    void (*drawLines)(int32_t x, const uint16_t* text, int32_t y, int32_t width, int32_t color, int32_t centered);  // D2Win+0x12940 (x in EAX)
};

// 1.13c D2Win+0x0A0E0 (fastcall, ECX = the list-box control; type 0xC, else fatal 0xD0). Control dwords: [2] flags (both Fog bit
// masks 0 and 2 needed), [3] / [4] x / y origin, [6] bottom, [0x10] first item, [0x11] selected item, [0x12] first visible index,
// [0x14] font index. The items chain through item+0x34 and each starts with its text (a Windows string). A control that is not enabled,
// or is empty, returns 0. Otherwise the font is selected and the 24 items from the first visible index are drawn in cells 0x78 wide and
// 0x14 tall, six per row: a small 0x1C-wide swatch (colour 0x55, mode 5), then the text at x + 0x20; the selected item also gets an
// outline (colour 0xFF, mode 1). Returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_A0E0(int32_t* c, const D2WinA0E0Env* e) {
    if (c[0] != 0xC) fatal(0xD0);
    const uint32_t flags = (uint32_t)c[2];
    if (!((kBm2 & flags) && (kBm0 & flags))) return 0;
    uint32_t item = (uint32_t)c[0x10];
    if (item == 0) return 0;
    e->setFont(c[0x14]);
    uint16_t buf[0x100] = {};   // the 256 default-constructed Unicode characters
    const int32_t baseY = (int32_t)font_height(e->fontTable, *e->fontIndex) - c[6] + c[4];
    int32_t i = 0;
    for (; item != 0; item = rd32(ptr(item), 0x34), ++i) {
        if (i < c[0x12] || i >= c[0x12] + 0x18) continue;
        const int32_t x = (i % 6) * 0x78 + c[3];
        const int32_t y = baseY + (i / 6) * 0x14;
        e->fillRect(x, y - 0xE, x + 0x1C, y, 0x55, 5);
        e->toUnicode(buf, (const char*)ptr(item), 0x100);
        e->drawLines(x + 0x20, buf, y, 0, 0, 0);
        if (item == (uint32_t)c[0x11]) e->fillRect(x - 1, y - 0x10, x + 0x76, y + 1, 0xFF, 1);
    }
    return 1;
}
