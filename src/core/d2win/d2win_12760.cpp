// src/core/d2win/d2win_12760.cpp -- 1.13c D2Win.dll+0x12760, draw UTF-16 text with colour codes (D2Gfx #10019)
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

struct D2Win12760Env {
    uint32_t fontCell;                    // *(*0x6F9014F0)
    const uint32_t* fontIndex;            // 0x6F8FFD34
    const uint32_t* fontTable;            // 0x6F901518, 5 dwords per entry
    const uint16_t* colorTag;             // 0x6F9AA670, the L"c" after 0xFF
    const uint8_t* (*glyph)(uint16_t c);  // *0x6F901508
    // D2Lang Unicode::strncmp (a in ECX, b in EDX, n on the stack); 0 when equal.
    int32_t (*strncmp16)(const uint16_t* a, const uint16_t* b, int32_t n);
    // D2Win+0x11F00 (text in EAX): width of the line starting at text.
    int32_t (*lineWidth)(const uint16_t* text);
    // D2Gfx #10019 (gfx block, x, y, gamma -1, draw mode, colour).
    void (*drawCelColor)(uint8_t* gfx, int32_t x, int32_t y, int32_t gamma, int32_t mode, int32_t color);
};

// 1.13c D2Win+0x12760 (x in EAX; text, y, width, colour, centre, mode on the stack). Centred
// lines start at x + (width - lineWidth) >> 1. "\xFFc<d>" sets the colour to d - '0', 0 when that
// is 13 or more; a negative value is kept, as in retail; a 0xFF not followed by 'c' is drawn as a
// glyph. A tag cut off by the end of the text sets the colour to 0. '\n' moves y up by
// height*16/10 (truncated toward zero).
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_12760(int32_t x, const uint16_t* text, int32_t y, int32_t width,
                                                    int32_t color, int32_t center, int32_t mode,
                                                    const D2Win12760Env* env) {
    int32_t len = u16len(text);
    int32_t x0 = x;
    if (center) x += (width - env->lineWidth(text)) >> 1;
    uint8_t gfx[0x48];
    std::memset(gfx, 0, sizeof gfx);
    wr32(gfx, 0x34, env->fontCell);
    int32_t i = 0;
    while (i < len) {
        uint16_t c = *text++;
        ++i;
        if (c < 0x100 && c == 0xFF) {
            uint16_t tag[2] = {*text, 0};   // retail stores the one unit into a dword stack slot
            ++i; ++text;
            if (env->strncmp16(tag, env->colorTag, 1) == 0) {
                if (i >= len) { color = 0; continue; }
                color = (int32_t)*text - 0x30;
                ++i; ++text;
                if (color >= 0xD) color = 0;
                continue;
            }
            --i; --text;
        } else if (c == 0x0A) {
            x = center ? x0 + ((width - env->lineWidth(text)) >> 1) : x0;
            int32_t hgt = (int32_t)font_height(env->fontTable, *env->fontIndex);
            y += (-(hgt << 4)) / 10;
            continue;
        }
        const uint8_t* g = env->glyph(c);
        wr32(gfx, 0, rd16(g, 8));
        env->drawCelColor(gfx, x, y, -1, mode, color);
        x += rd8(g, 3);
    }
}
