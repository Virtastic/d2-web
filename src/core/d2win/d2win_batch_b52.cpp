// src/core/d2win/d2win_batch_b52.cpp -- 1.13c D2Win.dll alpha batch (no spec): 0x8B50, 0xBD70, 0x16990.
// Transcribed from the cached decompile. Retail globals/imports come in through an env struct (seams).
#include <cstdint>
#include <unistd.h>
#include <emscripten.h>

struct D2WinB52Env {
    int32_t (*getMode)();                 // D2Win ordinal 10009
    int32_t (*setFont)(int32_t);          // 0x12FE0, returns previous font index
    const int32_t* curFont;               // 0x6F8FFD34
    const uint8_t* const* fontTab;        // 0x6F901518, stride 5 dwords; entry byte +10 = font size class
    int32_t* cursorState;                 // 0x6F900C68
    const int32_t* flag;                  // 0x6F9A9CE8
    // BD70
    uint8_t (*palIndex)(const void* pal, int32_t n, uint8_t r, uint8_t g, uint8_t b); // D2Gfx/CMP #10049
    const void* pal;                      // 0x6F922338
    const uint8_t* rgbA; const uint8_t* rgbB; // 0x6F8FFE9C, 0x6F8FFEBC (10 triples each)
    uint8_t* outA; uint8_t* outB;         // 0x6F9A8C8C, 0x6F9A9CA8
    // 16990
    void** pentSpin;                      // 0x6F9014EC
    void* (*loadCel)(const char* path, int32_t);  // 0xA7A0
    void (*fatal)(int32_t id);            // 0x76A2/0x7684 path, string 0x103; does not return
    int (*fmt)(char* buf, const char* f, const char* a);   // wsprintfA
};

// 1.13c D2Win+0x08b50
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_8b50(int32_t font, const D2WinB52Env* e) {
    auto cls = [&]() { return e->fontTab[*e->curFont * 5][10]; };
    int32_t m = e->getMode();
    if (m == 7) {
        int32_t prev = e->setFont(font);
        *e->cursorState = cls() > 0xd ? 8 : (cls() > 7) + 6;
        e->setFont(prev);
        return;
    }
    if (e->getMode() == 9) {
        int32_t prev = e->setFont(font);
        *e->cursorState = cls() < 0xe ? (cls() > 7) + 6 : 8;
        if (*e->flag) *e->cursorState = 3;
        e->setFont(prev);
        return;
    }
    m = e->getMode();
    if (m == 6 || e->getMode() == 10) { *e->cursorState = *e->flag ? 0 : 4; return; }
    *e->cursorState = 4;
}

// 1.13c D2Win+0xBD70
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_bd70(const D2WinB52Env* e) {
    for (int i = 0; i < 10; ++i) e->outA[i] = e->palIndex(e->pal, 0x100, e->rgbA[i*3], e->rgbA[i*3+1], e->rgbA[i*3+2]);
    for (int i = 0; i < 10; ++i) e->outB[i] = e->palIndex(e->pal, 0x100, e->rgbB[i*3], e->rgbB[i*3+1], e->rgbB[i*3+2]);
}

// 1.13c D2Win+0x16990
extern "C" EMSCRIPTEN_KEEPALIVE
void d2win_sub_16990(const D2WinB52Env* e) {
    char path[260];
    if (*e->pentSpin) { e->fatal(0x103); _exit(-1); }
    e->fmt(path, "%s\\UI\\CURSOR\\Pentspin", "DATA\\GLOBAL");
    *e->pentSpin = e->loadCel(path, 0);
}
