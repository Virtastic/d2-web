// src/core/d2win/d2win_fa80.cpp -- 1.13c D2Win.dll+0x0FA80, button (type 6) draw
// Decompile-derived from the retail bytes (retail/D2/D2SE/CORES/1.13c/D2Win.dll, image base
// 0x6F8E0000) and the cached Ghidra decompile or, where Ghidra defines no function, the
// disassembly. Imported callees and D2Win state are seams on the Env struct, passed last, so the
// retail argument order is preserved. Prior art: D2MOO 1.10f reference/D2MOO/source/D2Win/
// (control type numbering and header layout only).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2win_open_util.h"
using namespace d2win_open;

struct D2WinFA80Env {
    const int32_t* mouseX;     // 0x6F901488
    const int32_t* mouseY;     // 0x6F90148C
    const uint32_t* modal;     // 0x6F9014B8
    uint32_t* depth;           // 0x6F9014A8, callback nesting counter
    // D2Win+0x0F5B0 (ctl in EAX, mouse x in ESI, mouse y in EDI): the hover test.
    int32_t (*hover)(uint8_t* ctl, int32_t mx, int32_t my);
    // D2Gfx #10041 (gfx block, x, y, gamma -1, draw mode, 0).
    void (*drawCel)(uint8_t* gfx, int32_t x, int32_t y, int32_t gamma, int32_t mode, int32_t n0);
    // D2Win+0x0F8D0 (ctl in ESI, font on the stack): draw the caption.
    void (*caption)(uint8_t* ctl, int32_t font);
};

// 1.13c D2Win+0x0FA80.
// Frame: base [0x17]; enabled and visible (bits 2 and 0) adds [0x15] when pressed or held,
// otherwise [0x16] is added. With a cell file [1] and bit 2 the image is drawn in 256 x 256
// tiles, rows of 4 frames, the rows above the last bottom-up from y - h + 1 + 256, the last at y;
// then the caption font is picked from the height (and [0x18] set to 9 or 10, [0x99] to 0) and
// drawn when the caption is non-empty. The press counter [0x13] advances (2 releases it into
// [0x12] = 1). The callback [0x9A] gets a 0x20-byte message {ctl, 0, 0, hover}; retail leaves the
// other message dwords as stack leftovers, zero here. Always returns 1.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_FA80(uint8_t* ctl, const D2WinFA80Env* env) {
    if (rd32(ctl, 0) != 6) fatal(0x14D);
    uint32_t fl = rd32(ctl, 8);
    int32_t frame = (int32_t)rd32(ctl, 0x5C);
    if ((fl & kBm2) && (fl & kBm0)) {
        uint32_t held = rd32(ctl, 0x44);
        if (held) {
            if (rd8(ctl, 0x40) & 3) wr32(ctl, 0x48, 1);
            else wr32(ctl, 0x48, (uint32_t)env->hover(ctl, *env->mouseX, *env->mouseY));
        }
        if ((held && rd32(ctl, 0x48)) || rd32(ctl, 0x4C)) frame += (int32_t)rd32(ctl, 0x54);
    } else {
        frame += (int32_t)rd32(ctl, 0x58);
    }
    if (rd32(ctl, 4) && (fl & kBm2)) {
        int32_t mode = ((fl & kBm0) || (rd8(ctl, 0x40) & 0x20)) ? 5 : 1;
        int32_t h = (int32_t)rd32(ctl, 0x18), w = (int32_t)rd32(ctl, 0x14);
        int32_t x = (int32_t)rd32(ctl, 0xC);
        int32_t rows = ((h + 0xFF) >> 8) - 1;
        int32_t cols = (w + 0xFF) >> 8;
        uint8_t gfx[0x48];
        std::memset(gfx, 0, sizeof gfx);
        wr32(gfx, 0x34, rd32(ctl, 4));
        if (rows > 0) {
            int32_t ty = (int32_t)rd32(ctl, 0x10) - h + 1 + 0x100;
            int32_t f = frame;
            for (int32_t r = rows; r; --r, ty += 0x100, f += 4)
                for (int32_t i = 0, tx = x; i < cols; ++i, tx += 0x100) {
                    wr32(gfx, 0, (uint32_t)(f + i));
                    env->drawCel(gfx, tx, ty, -1, mode, 0);
                }
        }
        if (rows >= 0)
            for (int32_t i = 0, tx = x; i < cols; ++i, tx += 0x100) {
                wr32(gfx, 0, (uint32_t)(frame + rows * 4 + i));
                env->drawCel(gfx, tx, (int32_t)rd32(ctl, 0x10), -1, mode, 0);
            }
        int32_t font;
        h = (int32_t)rd32(ctl, 0x18);
        wr32(ctl, 0x60, 10);
        if (h >= 0x23) { wr32(ctl, 0x60, 9); font = 4; }
        else if (h == 0x20) font = 3;
        else font = (h > 0x14) + 1;
        wr32(ctl, 0x264, 0);
        if (rd16(ctl, 0x64) != 0) env->caption(ctl, font);
    }
    uint32_t press = rd32(ctl, 0x4C);
    if (press == 2) { wr32(ctl, 0x4C, 0); wr32(ctl, 0x48, 1); }
    else if (press) wr32(ctl, 0x4C, press + 1);
    uint32_t cb = rd32(ctl, 0x268);
    if (cb) {
        if (*env->modal) {
            wr32(ctl, 0x26C, 0);
        } else {
            int32_t x = (int32_t)rd32(ctl, 0xC), y = (int32_t)rd32(ctl, 0x10);
            int32_t mx = *env->mouseX, my = *env->mouseY;
            int32_t in = mx >= x && my >= y - (int32_t)rd32(ctl, 0x18) &&
                         mx < (int32_t)rd32(ctl, 0x14) + x && my < y;
            wr32(ctl, 0x26C, (uint32_t)in);
        }
        uint32_t msg[8] = {0};
        msg[0] = slot(ctl);
        msg[3] = rd32(ctl, 0x26C);
        ++*env->depth;
        ((void (*)(uint32_t*))(uintptr_t)cb)(msg);
        --*env->depth;
    }
    return 1;
}
