// src/core/d2game/d2game_batch_75510.cpp -- 1.13c D2Game.dll sub_75510 transcribed from the cached
// Ghidra decompile (build-wasm/ghidra-d2game/decomp/0x75510.c). Callees are seams in retail argument
// order; the data-table count at sgptDataTables+0xA80 arrives as a value.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameBatch75510Env {
    uint32_t (*sub74F20)(uint32_t unit);              // 0x74F20, fastcall(unit)
    int32_t  (*o10800)(uint32_t id);                  // D2Common #10800 (0xAAF0)
    int32_t  (*sub18A0)();                            // 0x18A0
    void     (*o10334)(uint32_t id, uint32_t unit, uint32_t zero); // 0xAC70
    void     (*subDA750)();                           // 0xDA750
    void     (*o11090)(uint32_t unit, uint32_t value); // 0xA4A2
    int32_t  (*o10511)(uint32_t unit);                // 0xAABA
    void     (*subA0830)(uint32_t param1);            // 0xA0830
    int32_t  tableCount;                              // *(sgptDataTables + 0xA80)
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }

// 1.13c D2Game+0x75510
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_75510(uint32_t param1, const uint32_t* param2,
                                                 const D2GameBatch75510Env* env)
{
    const uint32_t unit = param2[1];
    if (env->sub74F20(unit) != 0) {
        const uint32_t id = rd32(unit, 0x2c);
        if (env->o10800(id) == 0) {
            const int32_t v = env->sub18A0();
            if (v >= 0 && v < env->tableCount && v == 0x6e) {
                env->o10334(id, unit, 0);
                const uint32_t a = param2[0];
                env->subDA750();
                env->o11090(unit, a);
                return 1;
            }
            return 0;
        }
        env->o10334(id, unit, 0);
    }
    const uint32_t a = param2[0];
    env->subDA750();
    env->o11090(unit, a);
    if (env->o10511(unit) != 0) env->subA0830(param1);
    return 1;
}
