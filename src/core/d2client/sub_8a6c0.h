#pragma once
// Shared by src/core/d2client/sub_8a6c0.cpp and its FN probe.
#include <cstdint>
struct D2c8A6C0Cell { uint32_t frame; uint32_t pad[12]; uint32_t cel; uint32_t pad2[4]; };  // 0x48 bytes
static_assert(sizeof(D2c8A6C0Cell) == 0x48, "cell ctx");
struct D2c8A6C0State {
    uint32_t gate;        // 0x11C3A4: nonzero -> do nothing
    uint32_t tickInit;    // 0x123708 bit0
    uint32_t tickBase;    // 0x123704
    uint8_t  baseInit;    // 0x123428
    uint32_t delta;       // 0x12342C
    uint32_t summitCel;   // 0x11BF1C
    uint32_t cloudCel;    // 0x11BF20
    uint8_t  cloudInit;   // 0x11BF28
    uint32_t seedLo, seedHi;  // *PTR_DAT_6fb91308
    int32_t  cloudPos[10];    // 0x10B9A0
    int32_t  cloudY[10];      // 0x10B548
    int32_t  cloudSpd[10];    // 0x10B570
    int32_t  viewW;           // 0xF9E14 (+0x170 -> scroll limit)
};
struct D2c8A6C0Env {
    uint32_t (*tick)(uint32_t self);                      // Ordinal_10651 (stdcall, one word: the caller's ECX)
    uint32_t (*loadCel)(const char* path);                // D2Client+0x2B420
    void (*drawCel)(D2c8A6C0Cell*, int32_t x, int32_t y, uint32_t color, uint32_t mode, uint32_t pal);  // Ordinal_10041
    int32_t (*renderMode)();                              // Ordinal_10031
    void (*seedInit)();                                   // Ordinals 10936/GetTickCount/10995/10614 (one-time)
};
