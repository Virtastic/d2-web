// 1.13c D2Game+0xE8200: mode-callback install for the unit EBX; EAX = owner unit or 0. Returns 1 when installed.
// Retail order: E2CD0(EAX = game) returns al; 33F80(ECX = game, stack game) runs when the quest list holds a type-0x12 node.
#include <emscripten.h>
#include "d2game_s3a_env.h"
using namespace s3a;
// 1.13c D2Game+0xE8200
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_E8200(uintptr_t owner, uintptr_t unit, const D2GameS3aEnv* env)
{
    uintptr_t rec = env->p(rd32(unit, 0x18));
    if (rd8(unit, 9) == 0 || rd8(rec, 0xc) == 0 || rd8(rec, 1) != 1) return 0;
    uint32_t game = rd32(unit, 4);
    if ((uint8_t)cl(env, 0xE2CD0, game, 0, 0, 0, 0)) return 0;
    if (quest(env, env->p(game), 0x12)) cl(env, 0x33F80, 0, game, 0, 0, 0, { game });
    wr32(unit, 0xc0, env->addr(0xE8010));
    wr8(rec, 1, 0);
    wr8(rec, 2, 1);
    wr32(rec, 4, owner ? rd32(owner, 0xc) : 0xffffffffu);
    return 1;
}
