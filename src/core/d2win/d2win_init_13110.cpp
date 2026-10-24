// src/core/d2win/d2win_init_13110.cpp -- 1.13c D2Win.dll 0x13110 (ordinal 10026), transcribed from
// the cached Ghidra decompile. Alpha batch lane: implementation + marker only, no spec, no vector.
// Font-system init: the language (Ordinal 10009) picks the font sub-directory string, three font
// indices and the glyph-draw routine; then loads the main and MonsterIndicators fonts once,
// clears the 0x800-byte cache and stores the 'c'/'m' color-code characters as Unicode.
#include <cstdint>
#include <cstring>

extern "C" {
extern uint8_t* (*d2win_glyph_lookup)(uint16_t);
uint8_t* d2win_glyph_121d0(uint16_t);
uint8_t* d2win_glyph_12150(uint16_t);
}
struct D2WinInit13110Env {
    int32_t (*getLanguage)();                          // Ordinal 10009
    uint8_t* dirName;                                  // 0x6F9014F4, 8 bytes
    uint8_t* flagLatin;                                // 0x6F9A9D12
    uint32_t* fontIdx;                                 // 0x6F8FFD38, 3C, 40
    void** mainFont;                                   // 0x6F901524
    void** monsterFont;                                // 0x6F901628
    void (*sub12350)(int32_t);                         // builds the font path in the caller's buffer
    void* (*loadFont)(const char* path, int32_t);      // 0x6EA7A0
    void (*sub12B40)();
    uint32_t* cache;                                   // 0x6F9A9E58, 0x200 dwords
    void*** fontPtrPtr;                                // 0x6F9014F0
    void* mainFontSlot;                                // &0x6F901524
    void (*toUnicode)(void* dst, const char* src, int32_t n);
    void* uniC;                                        // 0x6F9AA670
    void* uniM;                                        // 0x6F9AA66C
    char* pathBuf;                                     // 0x50-byte buffer shared with sub12350
    int (*wsprintf)(char*, const char*, ...);
};

// 1.13c D2Win+0x13110
void d2win_init_13110(D2WinInit13110Env* e) {
    int32_t lang = e->getLanguage();
    *e->flagLatin = 0;
    e->fontIdx[0] = 0x10; e->fontIdx[1] = 0x11; e->fontIdx[2] = 3;
    const char* dir = "Latin\\";
    bool latinDraw = false;
    if (lang < 6) { d2win_glyph_lookup = d2win_glyph_121d0; e->fontIdx[1] = 0x10; latinDraw = true; }
    else {
        switch (lang) {
        case 6: e->fontIdx[0] = 0xe; e->fontIdx[1] = 0xf; e->fontIdx[2] = 5; dir = "JPN\\"; break;
        case 7: e->fontIdx[2] = 5; dir = "KOR\\"; break;
        case 8: case 9: e->fontIdx[0] = 0xe; e->fontIdx[1] = 0xe; e->fontIdx[2] = 2; dir = "CHI\\"; break;
        case 10: e->fontIdx[0] = 0xd; e->fontIdx[1] = 0xd; dir = "LATIN2\\"; break;
        case 0xb: dir = "CYR\\"; break;
        case 0xc: *e->flagLatin = 1; dir = "LATIN\\"; d2win_glyph_lookup = d2win_glyph_121d0; latinDraw = true; break;
        default: break;
        }
        if (!latinDraw) d2win_glyph_lookup = d2win_glyph_12150;
    }
    std::memcpy(e->dirName, dir, std::strlen(dir) + 1);
    if (!*e->mainFont) {
        e->sub12350(0x4f);
        *e->mainFont = e->loadFont(e->pathBuf, 0);
        e->sub12B40();
    }
    if (!*e->monsterFont) {
        char p[260];
        e->wsprintf(p, "%s\\UI\\Font\\MonsterIndicators", "DATA\\GLOBAL");
        *e->monsterFont = e->loadFont(p, 0);
    }
    std::memset(e->cache, 0, 0x800);
    *e->fontPtrPtr = (void**)e->mainFontSlot;
    e->toUnicode(e->uniC, "c", 2);
    e->toUnicode(e->uniM, "m", 2);
}
