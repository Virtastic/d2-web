// src/core/d2win/d2win_9840.cpp -- 1.13c D2Win.dll scroll-list key/command handler, from the cached
// Ghidra decompile (build-wasm/ghidra-d2win/decomp/0x9840.c).
#include <cstdint>
#include <unistd.h>
#include <emscripten.h>

extern "C" uint32_t d2win_sub_88A0(int32_t* c, int32_t idx);

// Host seams: sub_9200 (visible line count), Fog bit-mask table (gdwBitMasks), DAT_6f9014b0 (capture flag).
struct D2Win9840Env {
    int32_t (*sub_9200)(int32_t* c);
    const uint32_t* bitMasks;
    uint32_t captureFlag;
};

// 1.13c D2Win+0x9840
// param_1[0] = list control (type 4), param_1[2] = command (0x21 page-up, 0x22 page-down, 0x23 end,
// 0x24 home, 0x26 up, 0x28 down). Control dwords: [2] flags, [0x15] count, [0x16] top, [0x18] selected
// (-1 = none), [0xd] change callback. Null control fatal 0x501, wrong type 0x502 (_exit(-1)).
// Returns 0. ponytail: 32-bit retail-layout callback pointer must fit 32 bits.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_9840(int32_t* param_1, const D2Win9840Env* env) {
    int32_t* c = (int32_t*)(intptr_t)param_1[0];
    if (!c || c[0] != 4) _exit(-1);
    if (!((env->bitMasks[2] & c[2]) && (env->bitMasks[0] & c[2]))) return 0;
    int32_t cnt = c[0x15];
    if (cnt < 1) return 0;
    int32_t sel0 = c[0x18];
    if (sel0 == -1 || env->captureFlag) return 0;
    int32_t top = c[0x16];
    int32_t lines = env->sub_9200(c);
    int32_t newTop = top, sel = sel0;
    switch (param_1[2]) {
    case 0x21:
        sel = top;
        if (sel0 <= top && top > 0) {
            newTop = top - lines + 1;
            newTop &= (newTop < 1) - 1;
            sel = newTop;
        }
        break;
    case 0x22: {
        int32_t last = lines - 1 + top;
        if (sel0 < last) {
            sel = cnt - 1;
            if (last < sel) sel = last;
        } else {
            int32_t lim = cnt - lines;
            if (top < lim) {
                if (last > lim) last = lim;
                sel = lines - 1 + last;
                newTop = last;
                if (cnt - 1 <= sel) sel = cnt - 1;
            }
        }
        break;
    }
    case 0x23: newTop = cnt - lines; sel = cnt - 1; break;
    case 0x24: newTop = 0; sel = 0; break;
    case 0x26:
        if (sel0 > 0) {
            sel = sel0 - 1; newTop = sel;
            if (top <= sel) {
                newTop = top;
                if (lines - 1 + top < sel) newTop = sel - lines + 1;
            }
        }
        break;
    case 0x28:
        if (sel0 < cnt - 1) {
            sel = sel0 + 1; newTop = sel;
            if (top <= sel) {
                newTop = top;
                if (lines - 1 + top < sel) newTop = sel - lines + 1;
            }
        }
        break;
    }
    c[0x18] = sel;
    if (lines < cnt && newTop != c[0x16]) d2win_sub_88A0(c, newTop);
    if (sel != sel0 && c[0xd]) ((void (*)(int32_t*))(intptr_t)c[0xd])(param_1);
    return 0;
}
