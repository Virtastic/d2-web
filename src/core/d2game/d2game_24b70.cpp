// D2Game+0x24B70: flag a monster as a unique-modifier carrier. EAX = unit, ECX = game. Callees arrive through the env.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game24B70Env {
    void     (*sub23260)(uint32_t eax, uint32_t esi);
    void     (*sub23CA0)(uint32_t s0, uint32_t s1, uint32_t s2);
    uint32_t (*subA478)(uint32_t unit);
    void     (*sub248A0)(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2);
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
static inline void     wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }
static inline uint8_t  rd8(uint32_t a, uint32_t o) { return *(const uint8_t*)(uintptr_t)(a + o); }
static inline void     wr8(uint32_t a, uint32_t o, uint8_t v) { *(uint8_t*)(uintptr_t)(a + o) = v; }

// 1.13c D2Game+0x24B70
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_24B70(uint32_t unit, uint32_t game, const D2Game24B70Env* env)
{
    if (unit == 0 || rd32(unit, 0) != 1) return;
    const uint32_t rec = rd32(unit, 0x14);
    if (rec == 0) return;
    wr8(rec, 0x16, (uint8_t)(rd8(rec, 0x16) | 1));
    env->sub23260(unit, game);
    env->sub23CA0(game, unit, 1);
    const uint32_t room = env->subA478(unit);
    env->sub248A0(0, 0, game, room, 0, 0);
    wr32(unit, 0xc4, rd32(unit, 0xc4) | 0x800);
    if (rd32(unit, 0) == 1) {
        const uint32_t md = rd32(unit, 0x14);
        if (md) wr8(md, 0x5c, (uint8_t)(rd8(md, 0x5c) | 1));
    }
}
