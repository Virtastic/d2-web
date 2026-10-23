// 1.13c D2Win+0xAF40 (392 B, __fastcall ECX=c): draw a type-5 (scrollbar) control. Seams via env.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

struct D2WinAF40Env {
    const uint32_t* bitMasks;                                  // gdwBitMasks
    uint32_t captureFlag;                                      // DAT_6f9014c4
    void (*sub_AD60)(void* args);                              // {c, 0x201}
    void (*draw)(void* cell, int32_t x, int32_t y, uint32_t color, uint32_t mode, uint32_t z); // #10041
};

// 1.13c D2Win+0x0AF40
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_AF40(int32_t* c, const D2WinAF40Env* env) {
    if (!c || c[0] != 5) _exit(-1);  // Fog #10265 -> #10024 -> _exit(-1)
    uint32_t f = (uint32_t)c[2];
    if (!(env->bitMasks[2] & f) || !(env->bitMasks[0] & f)) return 0;
    int32_t x = c[3], base = c[1], y = c[4] - 1;
    if (!c[0x12]) return 1;
    if (!env->captureFlag) c[0x16] = 0;
    if (c[0x16]) { uintptr_t a[2] = {(uintptr_t)c, 0x201}; env->sub_AD60(a); }
    // retail's 6-dword cell overlays the zeroed 18-dword cell on the stack: one contiguous buffer
    uint32_t b[24] = {};
    uint32_t* cell = b + 6;
    cell[0] = (c[0x11] != 0) * 2 + 1;
    cell[13] = (uint32_t)base;
    env->draw(cell, x, y, 0xffffffff, 5, 0);
    b[0] = 5;
    if (10 < c[6] - 10) { int32_t i = 10; do { y -= 10; i += 10; env->draw(b, x, y, 0xffffffff, 5, 0); } while (i < c[6] - 10); }
    b[0] = c[0x10] ? 2 : 0;
    env->draw(b, x, (c[4] - c[6]) + 9, 0xffffffff, 5, 0);
    env->draw(b, x, (int32_t)(((int64_t)(c[6] - 0x1e) * c[0x13]) / c[0x12]) - c[6] + 0x13 + c[4], 0xffffffff, 5, 0);
    return 1;
}
