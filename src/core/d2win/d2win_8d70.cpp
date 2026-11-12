// src/core/d2win/d2win_8d70.cpp -- 1.13c D2Win.dll+0x08D70, text-box (type 4) draw: a backdrop cell, then rows of up to four columns.
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll) and the cached Ghidra decompile, resolved against
// the disassembly (the decompile loses the register arguments of the sub-calls). Callees are seams on the Env struct.
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2Win8D70Env {
    const uint32_t* fontIndex;                                                                      // 0x6F8FFD34
    const uint32_t* fontTable;                                                                      // 0x6F901518, 5 dwords per entry
    const uint8_t* palette;                                                                         // 0x6F922338
    uint8_t (*nearestColor)(const uint8_t* palette, int32_t n, int32_t r, int32_t g, int32_t b);    // D2CMP #10049
    void (*drawCell)(int32_t* cell, int32_t x, int32_t y, int32_t a, int32_t b, int32_t c);         // D2Gfx #10041 (a 0x48-byte cell record)
    void (*setFontExtra)(int32_t v);                                                                // D2Win+0x8B50 (EAX)
    void (*setFont)(int32_t index);                                                                 // D2Win+0x12FE0 (ECX)
    int32_t (*lineWidth)(const uint16_t* text);                                                     // D2Win+0x12B20 (ECX)
    int32_t (*textWidth)(const uint16_t* text, int32_t n);                                          // D2Win+0x12080 (ECX, EDX)
    void (*fitText)(uint16_t* text, int32_t width);                                                 // D2Win+0x8790
    void (*drawBox)(const uint16_t* text, int32_t x, int32_t y, int32_t width, int32_t fillColor, int32_t fillMode, int32_t textColor);  // D2Win+0x12D10
    void (*drawLines)(int32_t x, const uint16_t* text, int32_t y, int32_t width, int32_t color, int32_t centered);  // D2Win+0x12940 (x in EAX)
    void (*drawTicker)(int32_t x, const uint16_t* text, int32_t y, int32_t color, int32_t scroll, int32_t width);  // D2Win+0x11A50 (x in EAX, text in ECX)
};

// 1.13c D2Win+0x08D70 (fastcall, ECX = the text-box control; NULL is fatal 0x3BC, a type word other than 4 fatal 0x3BD).
// Control dwords: [1] backdrop cell, [2] flags (both Fog bit masks 0 and 2 needed, else returns 0), [3]/[4] x/y, [5]/[6] width/height,
// [0x10]/[0x11] x/y margin, [0x14] first row, [0x16] first row id, [0x17] style flags (1 highlight, 2 centre, 0x10 right, 0x40 ticker,
// 0x80 scroll up), [0x18] highlighted row id, [0x19] column count, [0x1A + c] column x offset (0 = flush), [0x1B + c] its right edge,
// [0x1F + c] per-column alignment, [0x25] font, [0x27] scroll, [0x28] / [0x29] row gap (row byte +0x18 set / clear), [0x2A] line pad.
// A row is: dwords 0..3 column strings (wide, NUL ends the row), [5] colour, byte +0x18, [7] next row. Rows are drawn until the
// height left cannot hold the next one; returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_8D70(int32_t* c, const D2Win8D70Env* e) {
    if (c == nullptr) fatal(0x3BC);
    if (c[0] != 4) fatal(0x3BD);
    const uint32_t flags = (uint32_t)c[2];
    if (!((kBm2 & flags) && (kBm0 & flags))) return 0;
    const uint8_t fillColor = e->nearestColor(e->palette, 0x100, 0x40, 0x40, 0x40);
    if (c[1] != 0) {
        int32_t cell[0x12] = {};
        cell[0xD] = c[1];
        e->drawCell(cell, c[3], c[4] - 1, -1, 5, 0);
    }
    e->setFontExtra(c[0x25]);
    e->setFont(c[0x25]);
    const int32_t left = c[0x10] + c[3];
    int32_t y = (int32_t)font_height(e->fontTable, *e->fontIndex) + c[4] - c[6] + c[0x11];
    e->setFontExtra(c[0x25]);
    const int32_t lineH = (int32_t)font_height(e->fontTable, *e->fontIndex) + c[0x2A];
    int32_t remain = c[6] - c[0x11] * 2;
    const int32_t width = c[5] - c[0x10] * 2;
    const uint32_t F = (uint32_t)c[0x17];
    if ((F & 0x40) && c[0x14] != 0) {
        c[0x27] -= 2;
        const int32_t w = e->lineWidth((const uint16_t*)ptr(rd32(ptr((uint32_t)c[0x14]), 0)));
        if (c[0x27] < -w) c[0x27] = width;
    }
    if ((int8_t)F < 0 && c[0x14] != 0) {
        const int32_t v = c[0x27] - 2;
        c[0x27] = v;
        if (v < -lineH) {
            c[0x27] = 0;
            c[0x14] = (int32_t)rd32(ptr((uint32_t)c[0x14]), 0x1C);
        }
    }
    uint32_t head = (uint32_t)c[0x14];
    int32_t row = 0;
    while (head != 0) {
        const uint8_t* hp = ptr(head);
        const int32_t cols = c[0x19];
        for (int32_t col = 0; col < cols; ++col) {
            const uint32_t sp = rd32(hp, (uint32_t)col * 4);
            if (sp == 0) break;
            const uint16_t* str = (const uint16_t*)ptr(sp);
            int32_t w = c[5];                                  // the column's width
            if (col < cols - 1) w = c[0x1B + col];
            const int32_t off = c[0x1A + col];
            int32_t x;
            if (off != 0) {
                x = left + off;
                w = (col + 1 < cols) ? c[0x1B + col] - off : c[5] - off;
                const uint32_t cf = (uint32_t)c[0x1F + col];
                if (cf & 2) {
                    int32_t half = (w - e->textWidth(str, u16len(str))) / 2;
                    if (half < 0) half = 0;
                    x += half;
                } else if (cf & 0x10) {
                    int32_t tw = e->textWidth(str, u16len(str));
                    if (tw < 0) tw = 0;
                    x += w - tw;
                }
            } else if (F & 2) {
                const uint16_t* s0 = (const uint16_t*)ptr(rd32(hp, 0));
                int32_t half = (c[5] - e->textWidth(s0, u16len(s0))) / 2;
                if (half < 0) half = 0;
                x = c[3] + half;
            } else if (F & 0x10) {
                const uint16_t* s0 = (const uint16_t*)ptr(rd32(hp, 0));
                int32_t tw = e->textWidth(s0, u16len(s0));
                if (tw < 0) tw = 0;
                x = c[3] - tw;
            } else {
                x = left;
            }
            uint16_t buf[0x400] = {};                          // the 1024 default-constructed Unicode characters
            u16cpy(buf, str);
            e->fitText(buf, w);
            const uint32_t colour = rd32(hp, 0x14);
            if (c[0x18] == c[0x16] + row && (F & 1)) {
                const int32_t tc = (int32_t)(colour == 4 ? 0 : colour);
                if (col == 0) e->drawBox(buf, x, y, width, fillColor, 5, tc);
                else e->drawLines(x, buf, y, 0, tc, 0);
            } else if (F & 0x40) {
                e->drawTicker(x, str, y, (int32_t)colour, c[0x27], width);
            } else {
                e->drawLines(x, buf, ((int8_t)F < 0) ? c[0x27] + y : y, 0, (int32_t)colour, 0);
            }
        }
        const int32_t step = (rd8(hp, 0x18) == 0 ? c[0x29] : c[0x28]) + lineH;
        y += step;
        remain -= step;
        ++row;
        head = rd32(hp, 0x1C);
        if (head == 0) break;
        if (remain < (rd8(ptr(head), 0x18) == 0 ? c[0x29] : c[0x28]) + lineH) break;
    }
    e->setFont(1);
    return 1;
}
