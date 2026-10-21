// src/core/d2client/stamina_bar_278b0.cpp -- 1.13c D2Client+0x278B0, the stamina bar and its hover text,
// from build-wasm/ghidra-d2client/decomp/0x278B0.c. Env-seam style (see d2client_batch_b5b86.cpp):
// retail globals are fields, unrecovered callees and imports are seams.
#include <cstdint>
#include <emscripten.h>

struct D2ClientStaminaEnv {
    uint32_t player;   // 0x6FBCBBFC
    int32_t  cursorX;  // 0x6FBCB828
    int32_t  cursorY;  // 0x6FBCB824
    int32_t  screenW;  // 0x6FB8BC48
    int32_t  screenH;  // 0x6FB8BC4C
    uint8_t  colRed, colYellow, colBlue; // 0x6FBCC4B8/9/A, 0 = not yet resolved

    int32_t  (*smoothedStamina)();                    // 0x26B20, current stamina (8.8 fixed)
    int32_t  (*getStat)(uint32_t unit, uint32_t stat);// Ordinal_10578, stat 11 = max stamina
    int32_t  (*hasStateA)(uint32_t unit, uint32_t st);// Ordinal_10866 (state 0x18)
    int32_t  (*hasStateB)(uint32_t unit, uint32_t st);// Ordinal_10494 (state 0x88)
    uint8_t  (*paletteIndex)(uint32_t r, uint32_t g, uint32_t b); // Ordinal_10190
    void     (*fillRect)(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color, uint32_t mode); // Ordinal_10014
    const uint16_t* (*getStringByIndex)(uint32_t index); // D2Lang #10003 (4164)
    void     (*unicodeDefaultCtor)(uint16_t* self);
    void     (*unicodeSprintf)(int32_t count, uint16_t* buf, const uint16_t* fmt, ...);
    void     (*setPopupUnicodeText)(const uint16_t* text, int32_t x, int32_t y, int32_t color, int32_t center); // D2Win #10085
};

// 1.13c D2Client+0x278B0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_278b0(D2ClientStaminaEnv* env) {
    const int32_t cur = env->smoothedStamina();
    const int32_t max = env->getStat(env->player, 11);
    if (env->colRed == 0) {
        env->colRed = env->paletteIndex(0xff, 0, 0);
        env->colYellow = env->paletteIndex(0xf4, 0xc0, 0x4c);
        env->colBlue = env->paletteIndex(0, 0, 0xff);
    }
    uint8_t col = env->colYellow;
    int32_t shown = max;
    if (max + 5 < cur || env->hasStateA(env->player, 0x18)) { shown = cur; col = env->colBlue; }
    int32_t width;
    if (shown < 1) { width = 0; col = env->colRed; }
    else {
        width = (cur * 0x66) / shown;
        if (width < 0x19) col = env->colRed;
    }
    const int32_t w = env->screenW, h = env->screenH;
    const int32_t left = w / 2 - 0x7f;
    env->fillRect(left, h - 0x1b, left + width, h - 9, col, 2);

    int32_t c8 = (cur + (cur >> 31 & 0xff)) >> 8;
    const int32_t m8 = (max + (max >> 31 & 0xff)) >> 8;
    uint32_t tcol = 0;
    if (c8 > m8 || env->hasStateB(env->player, 0x88)) { tcol = 3; c8 = m8; }

    uint16_t text[100];
    for (int32_t i = 0; i < 100; i++) env->unicodeDefaultCtor(&text[i]);
    // ponytail: decompile compares the bar colour reg against the x range (register reuse);
    // the hover box is taken as cursor x/y
    if (env->cursorX >= left && env->cursorX <= w / 2 - 0x19 &&
        env->cursorY >= h - 0x1b && env->cursorY <= h - 9) {
        env->unicodeSprintf(100, text, env->getStringByIndex(4164), c8, m8);
        env->setPopupUnicodeText(text, w / 2 - 76, h - 0x34, (int32_t)tcol, 1);
    }
}
