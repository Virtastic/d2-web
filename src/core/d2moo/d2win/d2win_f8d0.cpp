// src/core/d2win/d2win_f8d0.cpp -- 1.13c D2Win.dll+0xF8D0, draw a button's caption (and, with flag 0x40, its second caption line)
// Decompile-derived from the retail bytes and the cached Ghidra decompile, resolved against the disassembly
// (the decompile loses the centring arithmetic and the argument registers). Imported callees and D2Win state are seams
// on the Env struct, passed last, so the retail argument order is preserved.
#include <cstdint>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2WinF8D0Env {
    void (*setFont)(int32_t fontId);                                   // D2Win+0x12FE0 (ECX = font id)
    const uint32_t* fontTable;                                         // 0x6F901518, 5 dwords per entry
    const uint32_t* fontIndex;                                         // 0x6F8FFD34
    int32_t (*textWidth)(const uint16_t* text, int32_t len);           // D2Win+0x12080 (ECX = text, EDX = length)
    const uint16_t* (*langString)(uint16_t id);                        // D2Win+0x7780, the D2Lang #10003 thunk (CX = id)
    void (*drawText)(int32_t x, const uint16_t* text, int32_t y, int32_t width, int32_t color, int32_t center, int32_t mode);  // D2Win+0x12760
};

static inline int32_t half(int32_t v) { return v / 2; }    // cdq / sub / sar: truncates toward zero
static inline int32_t third(int32_t v) { return v / 3; }   // imul 0x55555556: truncates toward zero

// Entry (ESI = the control, dy on the stack, ret 4).
// 1.13c D2Win+0xF8D0
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_F8D0(const uint8_t* c, int32_t dy, const D2WinF8D0Env* e) {
    e->setFont((int32_t)rd32(c, 0x60));
    const int32_t fh = (int32_t)rd8(ptr(e->fontTable[*e->fontIndex * 5]), 0xA);
    const uint16_t* text = (const uint16_t*)(c + 0x64);
    const int32_t cx = (int32_t)rd32(c, 0xc), cy = (int32_t)rd32(c, 0x10), cw = (int32_t)rd32(c, 0x14), ch = (int32_t)rd32(c, 0x18);
    const int32_t color = (int32_t)rd32(c, 0x264);
    const bool pressed = (rd32(c, 0x44) != 0 && rd32(c, 0x48) != 0) || rd32(c, 0x4c) != 0;
    int32_t dx = half(cw - e->textWidth(text, u16len(text)));
    if (dx < 0) dx = 0;
    int32_t bx = cx + dx;
    int32_t di = cy - half(ch - fh) + dy;
    if (pressed) { bx -= 2; di += 2; }
    if (rd8(c, 0x40) & 0x40) {
        dx = half(cw - e->textWidth(text, u16len(text)));
        if (dx < 0) dx = 0;
        bx = cx + dx;
        di = cy - third(ch - fh) * 2 + dy;
        const uint16_t* s = e->langString(rd16(c, 0x270));
        int32_t dx2 = half(cw - e->textWidth(s, u16len(s)));
        if (dx2 < 0) dx2 = 0;
        int32_t sx = cx + dx2;
        int32_t sy = cy - third(ch - fh) + dy;
        if (pressed) { sx -= 2; bx -= 2; di += 2; sy += 2; }
        s = e->langString(rd16(c, 0x270));
        e->drawText(sx, s, sy, 0, color, 0, 4);
    }
    e->drawText(bx, text, di, 0, color, 0, 4);
}
