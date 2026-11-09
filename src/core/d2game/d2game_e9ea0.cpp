// 1.13c D2Game+0xE9EA0: EAX = unit. Unit-record kind 1: finds the game unit in the hash chain game+0x1520 keyed (id & 0x7f), advances its mode.
#include <emscripten.h>
#include "d2game_s3a_env.h"
using namespace s3a;
// 1.13c D2Game+0xE9EA0 (the game == 0 branch is the fatal-exit path and is not ported)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E9EA0(uintptr_t unit, const D2GameS3aEnv* env)
{
    uintptr_t rec = env->p(rd32(unit, 0x18));
    if (rd8(rec, 0) != 1) return;
    uint32_t gh = rd32(unit, 4);
    uintptr_t game = env->p(gh);
    uint32_t id = rd32(rec, 4);
    uint32_t cur = rd32(game, 0x1520 + (id & 0x7f) * 4);
    while (cur && rd32(env->p(cur), 0xc) != id) cur = rd32(env->p(cur), 0xe4);
    if (!cur) { wr32(rec, 8, 2); return; }
    uintptr_t u = env->p(cur);
    cl(env, 0xA4A2, 0, 0, 0, 0, 0, { cur, rd32(rec, 8) });
    if (rd32(rec, 8) == 2) return;
    wr32(rec, 8, 2);
    uintptr_t sk = cl(env, 0xA520, 0, 0, 0, 0, 0, { rd32(u, 4) });
    uint32_t t = (rd32(env->p((uint32_t)sk), 0xdc) >> 8) + rd32(game, 0xa8);
    cl(env, 0x8EA80, 0, gh, cur, 0, 0, { 1, t, 0, 0, 0 });
}
