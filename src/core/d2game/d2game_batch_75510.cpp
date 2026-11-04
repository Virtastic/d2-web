// src/core/d2game/d2game_batch_75510.cpp -- 1.13c D2Game.dll sub_75510. Fastcall: ECX = game,
// EDX = record {+0 value, +4 unit}. Callee seams carry the registers and stack words retail passes;
// the data-table count at sgptDataTables+0xA80 arrives as a value.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

#include "d2game_batch_75510.h"

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }

// 1.13c D2Game+0x75510
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_game_75510(uint32_t param1, const uint32_t* param2,
                                                 const D2GameBatch75510Env* env)
{
    const uint32_t unit = param2[1];
    if (env->sub74F20(param2[0], unit) != 0) {
        const uint32_t id = rd32(unit, 0x2c);
        if (env->o10800(id) == 0) {
            const int32_t v = env->sub18A0(unit);
            if (v >= 0 && v < env->tableCount && v == 0x6e) {
                env->o10334(id, unit, 0);
                const uint32_t a = param2[0];
                env->subDA750(param1);
                env->o11090(unit, a);
                return 1;
            }
            return 0;
        }
        env->o10334(id, unit, 0);
    }
    const uint32_t a = param2[0];
    env->subDA750(param1);
    env->o11090(unit, a);
    if (env->o10511(unit) != 0) env->subA0830(unit, param1);
    return 1;
}
