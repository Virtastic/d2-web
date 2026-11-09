// 1.13c D2Game+0xE8F30: fastcall ECX = game, EDX = unit. Scans the room under the unit's record for a free node; installs the E8140 mode callback.
#include <emscripten.h>
#include "d2game_s3a_env.h"
using namespace s3a;
// 1.13c D2Game+0xE8F30
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_E8F30(uintptr_t game, uintptr_t unit, const D2GameS3aEnv* env)
{
    uintptr_t rec = env->p(rd32(unit, 0x18));
    wr8(rec, 5, 0);
    if (rd8(rec, 0x18) != 1 || rd8(rec, 3) == 0 || rd8(rec, 2) != 0 || rd32(game, 0xc4) == 0) return 1;
    uint32_t room = (uint32_t)cl(env, 0xA526, 0, 0, 0, 0, 0, { rd32(game, 0xc4), rd32(rec, 8), rd32(rec, 0xc) });
    if (!room) return 1;
    uint32_t list = 0, count = 0;
    cl(env, 0xA55C, 0, 0, 0, 0, 0, { room, env->h((uintptr_t)&list), env->h((uintptr_t)&count) });
    for (uint32_t i = 0; i < count; i++) {
        uint32_t u = rd32(env->p(list), i * 4);
        for (uint32_t n = rd32(env->p(u), 0x74); n; n = rd32(env->p(n), 0xe8)) {
            if (rd8(env->p(n), 0) == 0) {
                cl(env, 0xE8780, 0, 0, 0, room, rd32(unit, 0x18), { (uint32_t)env->h(game) });
                wr32(unit, 0xc0, env->addr(0xE8140));
                return 1;
            }
        }
    }
    return 1;
}
