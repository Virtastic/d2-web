// src/core/d2game/d2game_batch_4eb90.cpp -- 1.13c D2Game.dll sub_4EB90 transcribed from the cached
// Ghidra decompile (build-wasm/ghidra-d2game/decomp/0x4EB90.c). Stage A batch lane: no spec, no vector.
// Callees are seams in retail argument order; the fatal path (Ordinal_10265(0xA2A) -> Ordinal_10024 ->
// _exit(-1)) is collapsed to the exit, as in the other batches.
#include <cstdint>
#include <unistd.h>
#include <emscripten.h>

struct D2Game4EB90Env {
    void (*fn_E0460)();                                  // 0xE0460
    void (*ordinal10945)(void* a, uint32_t b, uint32_t c); // Fog/D2Common #10945
    void (*fn_EFC60)();                                  // 0xEFC60
};

// 1.13c D2Game+0x4EB90
// __fastcall: param_1 in ECX, param_2 in EDX.
EMSCRIPTEN_KEEPALIVE void d2_game_4eb90(const D2Game4EB90Env* env, void* param_1, uint32_t param_2)
{
    env->fn_E0460();
    env->ordinal10945(param_1, param_2, 0);
    if (param_1 == nullptr) _exit(-1);
    env->fn_EFC60();
}
