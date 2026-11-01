// D2Game+0x268E0: walk a monster's nine unique-mod ids and call each id's callback from the 0x10F448 table.
// ECX = param1, EDX = unit, stack param3 then param4 (ret 8). Callbacks are retail code, so they arrive through the env.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game268E0Env {
    const uint8_t* table;   // DAT_6FD2F448: six dwords per id; a non-zero word names a handler
    void (*call)(uint32_t handler, uint32_t ecx, uint32_t edx, uint32_t id, uint32_t flag);
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }

// 1.13c D2Game+0x268E0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_268E0(uint32_t ecx, uint32_t unit, uint32_t param3, uint32_t param4,
                                                        const D2Game268E0Env* env)
{
    if (unit == 0 || rd32(unit, 0) != 1) return;
    const uint32_t rec = rd32(unit, 0x14);
    if (rec == 0) return;
    const uint32_t flag = (*(const uint8_t*)(uintptr_t)(rec + 0x16) >> 3) & 1;
    const uint8_t* ids = (const uint8_t*)(uintptr_t)(rec + 0x1c);
    if (ids[0] == 0) return;
    for (uint32_t i = 0; i < 9; ++i) {
        const uint32_t id = ids[i];
        // column 5 reads the same table but hands param3 to the callback in EDX instead of the unit
        const uint32_t slot = param4 == 5 ? id * 6 + 5 : id * 6 + param4;
        uint32_t cb; std::memcpy(&cb, env->table + slot * 4, 4);
        if (cb) env->call(cb, ecx, param4 == 5 ? param3 : unit, id, flag);
    }
}
