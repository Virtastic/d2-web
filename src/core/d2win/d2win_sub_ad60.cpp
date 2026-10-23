// src/core/d2win/d2win_sub_ad60.cpp -- 1.13c D2Win.dll scrollbar mouse handler, from the cached
// decompile (build-wasm/ghidra-d2win/decomp/0xAD60.c). Alpha lane: implementation + marker only.
#include <cstdint>
#include <emscripten.h>

extern "C" [[noreturn]] void d2_mcp_startup_overflow(uint32_t message);
extern "C" uint32_t d2win_sub_88A0(int32_t* c, int32_t idx);
// Env seams: D2Win globals DAT_6f901488 / DAT_6f90148c (mouse x / y).
extern "C" int32_t g_d2win_mouse_x_1488;
extern "C" int32_t g_d2win_mouse_y_148c;

// 1.13c D2Win+0x0AD60
// msg[0] = control (type 5 scrollbar, else fatal 0x12f), msg[1] = message (0x201 = left down).
// Control dwords: [2] flags, [3..6] rect x/y/w/h-ish, [0x10]/[0x11] up/down-arrow held,
// [0x12] max, [0x13] pos, [0x14] page step, [0x15] linked list control, [0x16] thumb grabbed,
// [0x17] extra cb, [0xd] chain cb. Links are 32-bit retail-layout pointers (ponytail: must fit 32 bits).
typedef uint32_t (*cb_t)(int32_t*);
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_AD60(int32_t* m) {
    int32_t* c = (int32_t*)(intptr_t)m[0];
    if (c[0] != 5) d2_mcp_startup_overflow(0x12f);
    if (m[1] != 0x201 || (c[2] & 4) == 0 || (c[2] & 1) == 0) return 1;
    int32_t max = c[0x12];
    if (max == 0) return 1;
    int32_t h = c[6];
    int32_t y = (h - c[4]) + g_d2win_mouse_y_148c;
    int32_t dx = g_d2win_mouse_x_1488 - c[3];
    if (dx < 0 || c[5] < dx) return 0;
    int32_t pos = c[0x13];
    int32_t trk = h - 0x1e;
    int32_t t = (pos * trk) / max;
    if (t + 10 <= y && y <= t + 0x14) c[0x16] = 1;
    if (y < 0xb && c[0x11] == 0 && c[0x16] == 0) {
        c[0x10] = 1; pos -= 1;
    } else if (h - 10 < y && c[0x10] == 0 && c[0x16] == 0) {
        c[0x11] = 1; pos += 1;
    } else {
        if (c[0x11] != 0 || c[0x10] != 0) goto clamp;
        if (c[0x16] != 0) { c[0x13] = ((y - 0xb) * max) / trk; goto clamp; }
        int32_t step = c[0x14];
        if (step == 0) {
            step = trk / 10;
            if (step == 0) step = 1;
            step = c[0x12] / step;
            if (step == 0) step = 1;
        }
        int32_t t2 = (pos * trk) / c[0x12];
        if (y < t2 + 10) pos -= step;
        else if (y <= t2 + 0x14) goto clamp;
        else pos += step;
    }
    c[0x13] = pos;
clamp:
    if (c[0x13] < 0) c[0x13] = 0;
    else if (c[0x12] < c[0x13]) c[0x13] = c[0x12];
    if (c[0x15] && m[1] == 0x201) d2win_sub_88A0((int32_t*)(intptr_t)c[0x15], c[0x13]);
    if (c[0x17]) ((cb_t)(intptr_t)c[0x17])(m);
    if (!c[0xd]) return 0;
    return ((cb_t)(intptr_t)c[0xd])(m);
}
