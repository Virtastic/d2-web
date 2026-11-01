// 1.13c D2Game+0x22FA0 -- quest/unique re-init on a type-1 unit (ECX = unit, [esp+4] = param, ret 4).
// The unit's +0x80 game record must exist and have byte +0x6D below 3. param 0: 0x21FB0(EAX = level+1) then
// 0x21EC0(EBX = unit, EDI = result). param != 0: 0x22A80(EAX = unit, mask 4) picks level+4 (hit) or level+7,
// then the same two calls and D2Common #10887(unit, 0x4A, 0, 0). Callees are seams in retail register order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game22FA0Env {
    uint32_t (*sub21FB0)(uint32_t n);
    void (*sub21EC0)(uint32_t unit, uint32_t pct);
    uint32_t (*sub22A80)(uint32_t unit, uint32_t mask);
    void (*ordinal10887)(uint32_t unit, uint32_t a, uint32_t b, uint32_t c);
};
static inline uint32_t g22fa0_rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_22FA0(uint32_t unit, int32_t param, const D2Game22FA0Env* env)
{
    if (unit == 0 || g22fa0_rd32(unit, 0) != 1) return;
    const uint32_t game = g22fa0_rd32(unit, 0x80);
    if (game == 0) return;
    const uint32_t lvl = *(const uint8_t*)(uintptr_t)(game + 0x6D);
    if (lvl >= 3) return;
    if (param == 0) {
        env->sub21EC0(unit, env->sub21FB0(lvl + 1));
        return;
    }
    const uint32_t n = env->sub22A80(unit, 4) ? lvl + 4 : lvl + 7;
    env->sub21EC0(unit, env->sub21FB0(n));
    env->ordinal10887(unit, 0x4A, 0, 0);
}
