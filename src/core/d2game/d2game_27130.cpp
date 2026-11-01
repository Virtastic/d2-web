// D2Game+0x27130: Taunt's target filter (vendored as SKILLS_Callback_FindTargetForTaunt). ECX = game (unused),
// EDX = unit. A monster qualifies when 0x9F120 (EAX unit, special state 0xC) passes, 0xDFA90 (EAX unit, dead)
// fails, and its AI owner record (0x20D50: monster data +0x14 -> AI control +0x28; a zero +0x28 word reads as
// type -1) has type -1 or a non-zero id. 0x20D50 is pure memory and runs inline here.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game27130Env {
    uint32_t (*specialState)(uint32_t unit, uint32_t state);   // D2Game 0x9F120 (EAX unit, stack state, ret 4)
    uint32_t (*isDead)(uint32_t unit);                         // D2Game 0xDFA90 (EAX unit)
};

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }

// 1.13c D2Game+0x27130
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2game_27130(uint32_t game, uint32_t unit, const D2Game27130Env* env)
{
    (void)game;
    if (!unit || rd32(unit, 0) != 1) return 0;
    if (!env->specialState(unit, 0xc)) return 0;
    if (env->isDead(unit)) return 0;
    // 0x20D50 (EAX unit, EDX &type, stack &id): the unit is a monster here
    const uint32_t md = rd32(unit, 0x14);
    const uint32_t ai = md ? rd32(md, 0x28) : 0;
    if (rd32(ai, 0x28) == 0) return 1;   // type := -1 (the id out-word is left unwritten and unread)
    const int32_t type = (int32_t)rd32(ai, 0x2c);
    const uint32_t id = rd32(ai, 0x30);
    return (type == -1 || id != 0) ? 1 : 0;
}
