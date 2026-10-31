// src/core/d2win/d2win_11960.cpp -- 1.13c D2Win.dll+0x11960, draw UTF-16 text glyph by glyph (D2Gfx #10041)
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

struct D2Win11960Env {
    uint32_t fontCell;                    // *(*0x6F9014F0): the current font's cell file
    const uint32_t* fontIndex;            // 0x6F8FFD34
    const uint32_t* fontTable;            // 0x6F901518, 5 dwords per entry
    // *0x6F901508 (char in ECX): the glyph record (frame u16 at +8, advance byte at +3).
    const uint8_t* (*glyph)(uint16_t c);
    // D2Gfx #10041 (gfx block, x, y, gamma -1, draw mode, 0).
    void (*drawCel)(uint8_t* gfx, int32_t x, int32_t y, int32_t gamma, int32_t mode, int32_t n0);
};

// 1.13c D2Win+0x11960 (x in EAX, text in ECX, y on the stack; reached through the 0x12720 thunk).
// 0xFF skips three code units (a colour code), '\n' returns to x and moves y up by height*15/10
// (truncated toward zero). Retail checks the length once and never counts down: on non-empty text
// the loop has no exit and walks past the terminator. That is transcribed as it stands.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_11960(int32_t x, const uint16_t* text, int32_t y,
                                                    const D2Win11960Env* env) {
    int32_t len = u16len(text);
    uint8_t gfx[0x48];
    std::memset(gfx, 0, sizeof gfx);
    wr32(gfx, 0x34, env->fontCell);
    if (len <= 0) return;
    int32_t cx = x;
    for (;;) {
        uint16_t c = *text++;
        if (c < 0x100 && c == 0xFF) { text += 2; continue; }
        if (c == 0x0A) {
            int32_t hgt = (int32_t)font_height(env->fontTable, *env->fontIndex);
            y += (hgt * -15) / 10;
            cx = x;
            continue;
        }
        const uint8_t* g = env->glyph(c);
        wr32(gfx, 0, rd16(g, 8));
        env->drawCel(gfx, cx, y, -1, 5, 0);
        cx += rd8(g, 3);
    }
}
