// src/core/d2game/d2game_batch_62700.cpp -- one 1.13c D2Game.dll function transcribed from the cached
// Ghidra decompile (build-wasm/ghidra-d2game/decomp/0x62700.c). Batch lane: implementation + marker only.
#include <cstdint>
#include <cstring>

// Seams in retail argument order: Ordinal_10920 (item -> record ptr), Ordinal_10174 (unit, a, b),
// FUN_6fc554f0 (D2Game+0x354F0, fastcall: ecx=param_2, edx=param_1).
struct D2Game62700Seams {
    void*    (*o10920)(void* item);
    uint32_t (*o10174)(uint32_t v, int32_t a, int32_t b);
    void     (*sub_354F0)(void* p2, void* p1);
};
static D2Game62700Seams g_seams62700;

// 1.13c D2Game+0x62700
uint32_t d2game_sub_62700(void* param_1, void* param_2) {
    const D2Game62700Seams& s = g_seams62700;
    const uint8_t* rec = (const uint8_t*)s.o10920(param_2);
    uint32_t v; std::memcpy(&v, rec + 0x10 + ((const uint8_t*)param_1)[0x6d] * 4, 4);
    if ((s.o10174(v, 6, 0) || s.o10174(v, 6, 0xf)) && !s.o10174(v, 6, 0xd) && !s.o10174(v, 6, 0xe))
        return 0;
    s.sub_354F0(param_2, param_1);
    return 0;
}
