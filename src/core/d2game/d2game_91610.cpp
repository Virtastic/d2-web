// 1.13c D2Game+0x91610 -- chance roll gated on a player unit.
// ESI = unit (null or type != 1 -> 0), stack (ret 8) = game and chance. Fails when 0xA7F6(unit) or 0xA514(unit) is non-zero or
// 0x9F120(EAX = unit, stack 11) returns 0; otherwise rolls rand(100) on the seed at game+0x20 and returns roll <= chance.
#include "d2game_8d6_common.h"
struct D2Game91610Env {
    uint32_t (*sub7F6)(uint32_t unit);              // 0xA7F6
    uint32_t (*sub514)(uint32_t unit);              // 0xA514
    uint32_t (*sub9F120)(uint32_t unit, uint32_t n);// 0x9F120 (EAX = unit, stack n)
    int32_t (*rand)(uint32_t seed, int32_t range);  // 0x1180 (ECX = seed, EAX = range)
};
// 1.13c D2Game+0x91610
uint32_t d2game_port_91610(uint32_t unit, uint32_t game, int32_t chance, const D2Game91610Env* env)
{
    if (!unit || d2g8::r32(unit, 0) != 1) return 0;
    if (env->sub7F6(unit)) return 0;
    if (env->sub514(unit)) return 0;
    if (!env->sub9F120(unit, 11)) return 0;
    return env->rand(game + 0x20, 100) <= chance ? 1 : 0;
}
