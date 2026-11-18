// src/core/d2win/d2win_sub_17eb0.cpp -- 1.13c D2Win.dll once-per-second tick, from the cached
// Ghidra decompile (build-wasm/ghidra-d2win/decomp/0x17EB0.c).
#include <cstdint>
#include <emscripten.h>

// Host seams: GetTickCount, D2Gfx #10080(&a,&b), thunk 0x7678 (0x72d,0), D2Gfx #10017(h),
// thunk 0x768A (0x73f,0), sub_17260 (screenshot writer), DAT_6f901484 (last tick).
struct D2Win17EB0Env {
    uint32_t (*getTickCount)();
    void (*gfx10080)(uint32_t* a, uint32_t* b);
    int32_t (*call7678)(int32_t a, int32_t b);
    int32_t (*gfx10017)(int32_t h);
    void (*sub_17260)(int32_t h);
    void (*call768A)(int32_t a, int32_t b);
    uint32_t* lastTick;
};

// 1.13c D2Win+0x17EB0
// If >= 1000 ms since *lastTick: query D2Gfx #10080 (results unused), h = thunk7678(0x72d,0); if
// D2Gfx #10017(h) != 0 run sub_17260(h); *lastTick = now; thunk768A(0x73f,0).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_17eb0(const D2Win17EB0Env* env) {
    uint32_t now = env->getTickCount();
    if (now - *env->lastTick < 1000) return;
    uint32_t a = 0, b = 0;
    env->gfx10080(&a, &b);
    int32_t h = env->call7678(0x72d, 0);
    if (env->gfx10017(h) != 0) env->sub_17260(h);
    *env->lastTick = now;
    env->call768A(0x73f, 0);
}
