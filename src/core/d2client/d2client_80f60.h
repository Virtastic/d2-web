// Globals and callees of the D2Client 0x80F60 weather particle step (port: d2client_80f60.cpp).
#pragma once
#include <cstdint>
struct D2c80f60G {
    int32_t rainMode;     // +0x11BF78
    uint32_t angle;       // +0x113918
    float f38f4;          // +0x1138F4
    int32_t tickDX, tickDY; // +0x11C408, +0x11C40C
    int32_t cellW;        // +0x0DBC48
    int32_t maxLive;      // +0x113934
    int32_t frame;        // +0x123440
    uint8_t* ctx;         // +0x11BF68 (+0x110 count, +0x114 live, +0x11C particle array)
};
struct D2c80f60Env {
    float (*cosF)(int32_t);                         // Fog 10083 (0xBFFE)
    float (*sinF)(int32_t);                         // Fog 10084 (0xBFE6)
    void (*ord10065)(uint32_t ecx, uint32_t edx);   // thunk 0xBFF8, ECX = ctx, EDX = index << 16
    void (*fn80700)(uint32_t param);                // 0x80700, one stack word
};
