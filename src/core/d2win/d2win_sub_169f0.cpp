// src/core/d2win/d2win_sub_169f0.cpp -- 1.13c D2Win.dll list-box (type 7) draw, from the cached
// Ghidra decompile (build-wasm/ghidra-d2win/decomp/0x169F0.c).
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

// Host seams: 0x12FE0 setFont, 0x12940 drawText, Ordinal_10041 drawCell, DAT_6f9014ec (cell color).
struct D2Win169F0Env {
    void (*setFont)(int32_t font);
    void (*drawText)(uint8_t* item, int32_t y, int32_t, int32_t, int32_t);
    void (*drawCell)(int32_t* rec, int32_t x, int32_t y, uint32_t a, uint32_t b, uint32_t c);
    int32_t cellColor;
};

// 1.13c D2Win+0x169F0
// c = control (type 7). Dwords: [3] left, [4] height, [5] width, [6] pad, [0x10] font, [0x11]/[0x12] row gaps,
// [0x13] first item index, [0x17] animation frame (0..7), [0x18] scroll origin, [0x19] item list head,
// [0x1a] selected item. Item byte offsets: +0x440 next, +0x410/0x418/0x420/0x428 stored rect (as decompiled).
// Wrong type: fatal (_exit(-1)). Returns 1. ponytail: 32-bit retail-layout pointers must fit 32 bits.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_169f0(int32_t* c, const D2Win169F0Env* env) {
    if (c[0] != 7) _exit(-1);
    env->setFont(c[0x10]);
    int32_t n = c[0x13];
    uint8_t* it = (uint8_t*)(uintptr_t)(uint32_t)c[0x19];
    int32_t y = (c[0x12] - c[6]) + c[0x11] + c[4];
    for (; n > 0; n--)
        if (it) it = (uint8_t*)(uintptr_t)*(uint32_t*)(it + 0x440);
    int32_t rec[18] = {0};
    rec[0] = c[0x17];
    rec[17] = 0;
    rec[13] = env->cellColor;
    for (; it && y < c[4]; y += c[0x11] + c[0x12] * 2) {
        if ((uint32_t)(uintptr_t)it == (uint32_t)c[0x1a]) {
            int32_t cy = y + 3;
            if (c[0x10] == 2) cy = y + 9;
            env->drawCell(rec, c[3] - 0x19 + ((c[5] - c[0x18]) >> 2), cy, 0xffffffffu, 5, 0);
        }
        env->drawText(it, y, 0, 0, 0);
        int32_t x = c[3];
        *(int32_t*)(it + 0x410) = x;
        *(int32_t*)(it + 0x418) = c[5] + x;
        *(int32_t*)(it + 0x420) = (y - c[0x12]) - c[0x11];
        *(int32_t*)(it + 0x428) = c[0x12] + y;
        it = (uint8_t*)(uintptr_t)*(uint32_t*)(it + 0x440);
    }
    if (++c[0x17] > 7) c[0x17] = 0;
    env->setFont(1);
    return 1;
}
