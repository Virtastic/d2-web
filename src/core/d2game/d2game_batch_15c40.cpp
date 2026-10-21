// src/core/d2game/d2game_batch_15c40.cpp -- 1.13c D2Game.dll 0x15C40 (candidate counter: unit type 1, class 0x1c5)
// transcribed from the cached Ghidra decompile. param_1 = game/arg for 0xE0A60, EBX (acc) = accumulator
// {+0 minA, +4 minB, +8 maxLvl, +0xC best1, +0x10 best2, +0x14 count}. Seams are the retail callees.
#include <cstdint>
#include <emscripten.h>

struct Seams {
    int32_t (*cls)(void*);              // 0x18A0 -> class id
    int32_t (*clsCount)();              // *(sgptDataTables+0xa80)
    int32_t (*o10648)(void*);
    uint32_t (*e0a60)(void*, int32_t);
    int32_t (*o10494)(void*, int32_t);
    int32_t (*v21900)();
    int32_t (*o11027)(void*);
    uint8_t (*f22a80)(int32_t);
};

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_15C40(int32_t param_1, void* param_2, int32_t* param_3, int32_t* acc, const Seams* s) {  // 1.13c D2Game+0x15C40
    if (param_2 == param_3 || !param_3 || *param_3 != 1) return 0;
    int32_t c = s->cls(param_3);
    if (!(c > -1 && c < s->clsCount() && c == 0x1c5)) return 0;
    if (s->o10648(param_3) != 0) return 0;
    if (s->e0a60(param_2, param_1) != 0) return 0;
    if (s->o10494(param_3, 0x8d) != 0) return 0;
    acc[5]++;
    if (acc[3] == 0 && s->v21900() <= acc[0] && s->o11027(param_3) < acc[2]) acc[3] = (int32_t)(intptr_t)param_3;
    if (acc[4] == 0 && s->v21900() <= acc[1] && s->f22a80(8) == 0) acc[4] = (int32_t)(intptr_t)param_3;
    return 0;
}
