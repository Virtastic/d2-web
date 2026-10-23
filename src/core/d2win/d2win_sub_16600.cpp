// src/core/d2win/d2win_sub_16600.cpp -- 1.13c D2Win.dll click/double-click dispatcher, from the cached
// Ghidra decompile (build-wasm/ghidra-d2win/decomp/0x16600.c).
#include <cstdint>
#include <unistd.h>
#include <emscripten.h>

// Host seams: mouse x/y (DAT_6f901488/8c), GetTickCount, last-tick global (DAT_6f9a9d00).
struct D2Win16600Env { int32_t mouseX, mouseY; uint32_t tick; uint32_t* lastTick; };

// 1.13c D2Win+0x16600
// param_1[0] = window (type 7). Window dwords: [0xd] default handler, [0x19] child list head,
// [0x1a] current child. Child: +0x204 handler, +0x208..0x214 L,R,T,B (exclusive), +0x220 next.
// Wrong type: fatal (_exit(-1)). ponytail: callbacks are 32-bit retail-layout pointers.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_16600(int32_t* param_1, const D2Win16600Env* env) {
    int32_t* w = (int32_t*)(intptr_t)param_1[0];
    if (w[0] != 7) _exit(-1);
    for (int32_t c = w[0x19]; c; c = *(int32_t*)(intptr_t)(c + 0x220)) {
        auto f = [&](int o) { return *(int32_t*)(intptr_t)(c + o); };
        if (f(0x208) < env->mouseX && env->mouseX < f(0x20c) && f(0x210) < env->mouseY && env->mouseY < f(0x214)) {
            if (c == w[0x1a] && env->tick - *env->lastTick < 300) {
                if (f(0x204)) return ((uint32_t(*)(int32_t*))(intptr_t)f(0x204))(param_1);
            } else w[0x1a] = c;
        }
    }
    *env->lastTick = env->tick;
    if (!w[0xd]) return 0;
    return ((uint32_t(*)(int32_t*))(intptr_t)w[0xd])(param_1);
}
