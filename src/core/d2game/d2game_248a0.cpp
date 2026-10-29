// D2Game+0x248A0 unique-monster-modifier applier.
// The callees and the two retail globals (mod handler table 0x6FD2E550, the 4 global mod ids at 0x6FD1CE7C) come in through the env.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game248A0Env {
    void           (*sub245A0)(uint32_t ecx, uint32_t edx);
    void           (*callMod)(uint32_t id);           // table[id](1) when the entry is non-null
    void           (*sub21490)(uint32_t id, uint32_t zero);
    const uint32_t* globalIds;                        // 4 entries
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }

// 1.13c D2Game+0x248A0 -- regs eax=flag ecx=param1 edx=param2 edi=unit.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_248A0_impl(uint32_t eax, uint32_t param1, uint32_t param2, uint32_t unit,
                                                  const D2Game248A0Env* env)
{
    if (eax != 0) env->sub245A0(param1, param2);
    for (int i = 0; i < 4; ++i) { env->callMod(env->globalIds[i]); env->sub21490(env->globalIds[i], 0); }
    if (unit == 0 || rd32(unit, 0) != 1) return;
    const uint32_t rec = rd32(unit, 0x14);
    if (rec == 0) return;
    for (uint32_t i = 0; i < 9; ++i) {
        const uint32_t id = *(const uint8_t*)(uintptr_t)(rec + 0x1c + i);
        if (id == 0) return;
        env->callMod(id);
        env->sub21490(id, 0);
    }
}
