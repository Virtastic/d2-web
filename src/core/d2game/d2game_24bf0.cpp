// D2Game+0x24BF0: append one unique-modifier id to a monster's record and re-apply the mods.
// EAX = unit, ECX = game, stack byte = mod id (ret 4). Callees arrive through the env.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game24BF0Env {
    void     (*sub23260)(uint32_t eax, uint32_t esi);
    uint32_t (*sub21860)(uint32_t ecx);                              // count of ids already in the list
    uint32_t (*subA478)(uint32_t unit);                              // the unit's room (1 stack word)
    void     (*sub248A0)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2);
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline uint8_t  rd8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
static inline void     wr8(uint32_t a, uint32_t o, uint8_t v) { *(uint8_t*)(uintptr_t)(a + o) = v; }

// 1.13c D2Game+0x24BF0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_24BF0(uint32_t unit, uint32_t game, uint32_t modId, const D2Game24BF0Env* env)
{
    if (unit == 0 || rd32(unit, 0) != 1) return;
    const uint32_t rec = rd32(unit, 0x14);
    if (rec == 0) return;
    if (rd8(rec, 0x16) & 4) return;
    env->sub23260(unit, game);
    if (rd32(unit, 0) == 1) {
        const uint32_t r2 = rd32(unit, 0x14);
        if (r2) wr8(r2, 0x16, (uint8_t)(rd8(r2, 0x16) | 5));
    }
    const uint32_t list = rec + 0x1c;
    const uint32_t n = env->sub21860(list);
    if ((int32_t)n >= 9) return;
    wr8(list, n, (uint8_t)modId);
    const uint32_t room = env->subA478(unit);
    env->sub248A0(1, 0, game, room, 0, 0);
}
