// src/core/d2win/d2win_11a50.cpp -- 1.13c D2Win.dll+0x11A50, draw UTF-16 text with colour codes between two x limits
// Decompile-derived from the retail bytes and the cached Ghidra decompile, resolved against the disassembly (argument
// registers and the clip tests). The 0x12760 drawing loop with a start offset and a right limit; imported callees and
// D2Win state are seams on the Env struct, passed last. Prior art: reference/D2MOO/source/D2Win/D2WinFont.cpp.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2Win11A50Env {
    uint32_t fontCell;                    // *(*0x6F9014F0)
    const uint32_t* fontIndex;            // 0x6F8FFD34
    const uint32_t* fontTable;            // 0x6F901518, 5 dwords per entry
    const uint16_t* colorTag;             // 0x6F9AA670, the L"c" after 0xFF
    const uint8_t* (*glyph)(uint32_t ecx);  // *0x6F901508; ecx = the offset's high half | the char
    int32_t (*strncmp16)(const uint16_t* a, const uint16_t* b, int32_t n);   // D2Lang Unicode::strncmp; 0 when equal
    // D2Gfx #10019 (gfx block, x, y, gamma -1, draw mode, colour), via 0x782E.
    void (*drawCelColor)(uint8_t* gfx, int32_t x, int32_t y, int32_t gamma, int32_t mode, int32_t color);
};

// Entry (x in EAX, text in ECX; y, colour, offset, limit on the stack; ret 0x10). Glyphs are drawn from
// x + offset while the pen is not past x + limit, and only when the pen is right of x. "\xFFc<d>" sets the colour to
// d - '0' (0 when 13 or more, or when the tag is cut off by the end); '\n' returns the pen to x and moves y by
// height*-16/10 (truncated toward zero).
// 1.13c D2Win+0x11A50
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_11A50(int32_t x, const uint16_t* text, int32_t y, int32_t color,
                                                    int32_t offset, int32_t limit, const D2Win11A50Env* e) {
    const int32_t len = u16len(text);
    uint8_t gfx[0x48];
    std::memset(gfx, 0, sizeof gfx);
    wr32(gfx, 0x34, e->fontCell);
    int32_t pen = x + offset;
    const int32_t right = x + limit;
    int32_t n = 0;
    while (n < len) {
        if (pen > right) return;
        const uint16_t c = *text++;
        ++n;
        if (c < 0x100 && c == 0xff) {
            uint16_t tag = *text++;
            ++n;
            if (e->strncmp16(&tag, e->colorTag, 1) == 0) {
                if (n < len) { color = (int32_t)*text++ - 0x30; ++n; if (color >= 13) color = 0; }
                else color = 0;
            } else {
                --n; --text;
                const uint8_t* g = e->glyph(((uint32_t)offset & 0xFFFF0000u) | c);
                wr32(gfx, 0, rd16(g, 8));
                if (pen > x) e->drawCelColor(gfx, pen, y, -1, 5, color);
                pen += rd8(g, 3);
            }
        } else if (c == 10) {
            pen = x;
            const int32_t h = (int32_t)rd8(ptr(e->fontTable[*e->fontIndex * 5]), 0xA);
            y += (-h * 16) / 10;
        } else {
            const uint8_t* g = e->glyph(((uint32_t)offset & 0xFFFF0000u) | c);
            wr32(gfx, 0, rd16(g, 8));
            if (pen > x) e->drawCelColor(gfx, pen, y, -1, 5, color);
            pen += rd8(g, 3);
        }
    }
}
