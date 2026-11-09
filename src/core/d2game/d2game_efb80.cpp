// 1.13c D2Game+0xEFB80: ECX = message id, EAX = unit, five stack words (s2, s3 unused), ret 0x14. Only message 0x154 with a unit acts: the
// unit's x/y (path +0xC/+0x10 for types 2, 4, 5; 0xA454/0xA3B8 of the path otherwise, 0 without one) go to 0xEF940 (EAX = D2Game+0x10F3C0,
// EBX = 8, ECX = s0, EDX = s4 (left from its push), stack s1, x, y, 0, 0x154, s4); returns its result, else 0.
#include <emscripten.h>
#include "d2game_s3a_env.h"
using namespace s3a;
// 1.13c D2Game+0xEFB80
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_EFB80(uint32_t msg, uintptr_t unit, uint32_t s0, uint32_t s1, uint32_t s4, const D2GameS3aEnv* env)
{
    if (msg != 0x154 || !unit) return 0;
    const uint32_t t = rd32(unit, 0);
    const bool sp = t == 2 || (t >= 4 && t <= 5);
    const uint32_t path = rd32(unit, 0x2c);
    const uint32_t x = sp ? rd32(env->p(path), 0xc) : path ? (uint32_t)cl(env, 0xA454, 0, 0, 0, 0, 0, { path }) : 0;
    const uint32_t y = sp ? rd32(env->p(path), 0x10) : path ? (uint32_t)cl(env, 0xA3B8, 0, 0, 0, 0, 0, { path }) : 0;
    return (uint32_t)cl(env, 0xEF940, env->addr(0x10F3C0), s0, s4, 8, 0, { s1, x, y, 0, 0x154, s4 });
}
