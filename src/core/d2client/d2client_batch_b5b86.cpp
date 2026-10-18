// src/core/d2client/d2client_batch_b5b86.cpp -- two 1.13c D2Client.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c and the retail disassembly (image base
// 0x6FAB0000). The first is an 8-byte CRT fragment; the second is the experience-bar / hover-text
// draw, which the decompiler renders with untyped `Ordinal_*` imports. Unrecovered internal callees
// and imported ordinals are host seams after the retail arguments / in the env struct (the
// d2client_batch_bf80.cpp and d2client_batch_1e070.cpp pattern). Retail globals are inputs, because
// the client platform layer owns them.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

// 1.13c D2Client+0x05B86 (8 B): `push ebx; call D2Client+0x0A71B; pop ecx; ret`. MSVC 2003 emitted
// this fragment out of line for __unlock_fhandle(_FileHandle) inside __write (D2Client+0x05AFF), where
// the handle is already live in EBX. The callee at 0xA71B does
// LeaveCriticalSection(&g_fhandleTable[_Filehandle >> 5][_Filehandle & 0x1F]); the handle is not a
// stack argument, so it is recovered as the explicit first parameter (unaff_EBX) and the critical
// section is a host seam.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_5b86(uint32_t fileHandle,
                                              void (*unlockFhandle)(uint32_t)) {
    unlockFhandle(fileHandle);
}

// Inputs and seams for D2Client+0x26970. Every global the function reads is a field, named at its
// retail RVA; every import it calls is a seam named at its D2Client veneer RVA.
struct D2ClientBatchB5B86Env {
    uint32_t player;    // 0x6FBCBBFC  live player unit (0 when there is none); +0x04 = class
    int32_t  cursorX;   // 0x6FBCB828
    int32_t  cursorY;   // 0x6FBCB824
    int32_t  screenW;   // 0x6FB8BC48
    int32_t  screenH;   // 0x6FB8BC4C

    int32_t  (*getPlayerStat)(uint32_t unit, uint32_t stat, uint32_t index);      // 0x0C1C6, D2Common!#10973
    int32_t  (*getPlayerBaseStat)(uint32_t unit, uint32_t stat, uint32_t index);  // 0x0C250, D2Common!#10587
    uint32_t (*getLevelThreshold)(int32_t cls, uint32_t level);                   // 0x0C24A, D2Common!#10399
    int32_t  (*getMaxLevel)(int32_t cls);                                         // 0x0C28C, D2Common!#10066
    const uint16_t* (*getStringByIndex)(uint32_t index);                          // 0x0D1EC, D2Lang!#10003 (index in ECX)
    void     (*unicodeDefaultCtor)(uint16_t* self);                               // 0x0CEB00 IAT, D2Lang Unicode::Unicode
    void     (*unicodeSprintf)(int32_t count, uint16_t* buf, const uint16_t* fmt, ...); // 0x0CEAB4 IAT, D2Lang Unicode::sprintf
    void     (*setPopupUnicodeText)(const uint16_t* text, int32_t x, int32_t y,
                                    int32_t color, int32_t center);               // 0x0D384, D2Win!#10085
    void     (*drawLine)(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                         uint32_t color, uint32_t mode);                          // 0x0D102, D2Gfx!#10010
};

// 1.13c D2Client+0x26970 (430 B): the experience bar and its hover text. Called every HUD frame.
// Reads: player 0x6FBCBBFC and player+0x04 (class); cursor x/y 0x6FBCB828/0x6FBCB824; screen W/H
// 0x6FB8BC48/0x6FB8BC4C. Calls D2Common!#10973 (get stat 12 level, stat 13 experience),
// D2Common!#10587 (base stat 12), D2Common!#10399 (level threshold), D2Common!#10066 (max level),
// D2Lang!#10003 (string 4163 "Experience: %u / %u"), D2Lang Unicode ctor/sprintf, D2Win!#10085
// (text, centred) and D2Gfx!#10010 (two 1-pixel bar lines).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_26970(const D2ClientBatchB5B86Env* env) {
    const uint32_t player = env->player;
    // [player+0x04] is the class; a null player leaves -1, which the table helpers fold to class 0.
    const int32_t cls = player ? (int32_t)*(const uint32_t*)(uintptr_t)(player + 4) : -1;

    const int32_t  level     = env->getPlayerStat(player, 12, 0);      // stat 12 = level
    const uint32_t experience = (uint32_t)env->getPlayerStat(player, 13, 0); // stat 13 = experience
    const int32_t  baseLevel = env->getPlayerBaseStat(player, 12, 0);

    // Next-level threshold uses the base level, the previous one the current level - 1.
    const uint32_t thresholdBase = env->getLevelThreshold(cls, (uint32_t)baseLevel);
    const uint32_t thresholdPrev = env->getLevelThreshold(cls, (uint32_t)(level - 1));
    const uint32_t span = thresholdBase - thresholdPrev;
    const uint32_t into = experience > thresholdPrev ? experience - thresholdPrev : 0;

    uint16_t text[100];
    for (int32_t i = 0; i < 100; i++) env->unicodeDefaultCtor(&text[i]);

    const int32_t w = env->screenW;
    const int32_t h = env->screenH;

    const bool hover = env->cursorX >= w / 2 - 146 && env->cursorX <= w / 2 - 23 &&
                       env->cursorY >= h - 43   && env->cursorY <= h - 34;
    if (hover) {
        const uint16_t* fmt = env->getStringByIndex(0x1043);         // string.tbl 4163
        env->unicodeSprintf(100, text, fmt, experience, thresholdBase);
        env->setPopupUnicodeText(text, w / 2 - 146, h - 51, 0, 1);
    }

    if (span != 0) {
        uint32_t numerator = into;
        uint32_t divisor = span;
        // Retail scales both down by 128 before the 32-bit multiply so the product cannot wrap.
        if (numerator >= 0x226B902u) { numerator >>= 7; divisor >>= 7; }
        uint32_t fill = (numerator * 0x77u) / divisor;
        if (fill > 0x77u) fill = 0;                                   // overflow guard: no bar
        if ((uint32_t)level < (uint32_t)env->getMaxLevel(cls) && fill != 0) {
            const int32_t x1 = w / 2 - 0x90;
            const int32_t y = h - 0x26;
            env->drawLine(x1, y, x1 + (int32_t)fill, y, 0xFF, 0xFF);
            env->drawLine(x1, y + 1, x1 + (int32_t)fill, y + 1, 0xFF, 0xFF);
        }
    }
}
