// 1.13c D2Game+0xE8FE0: quest-NPC event callback. ECX = q (quest record), EDX = msg (event block: +0 unit, +8 arg, +0xC game, +0x14 event, +0x18 sub-event).
#include <emscripten.h>
#include "d2game_s3a_env.h"
using namespace s3a;
// 1.13c D2Game+0xE8FE0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E8FE0(uintptr_t q, uintptr_t msg, const D2GameS3aEnv* env)
{
    const uint16_t sub = rd16(msg, 0x18), ev = rd16(msg, 0x14);
    const uint32_t unit = rd32(msg, 0), game = rd32(msg, 0xc), qh = env->h(q), arg = rd32(msg, 8);
    const uint32_t pl = (uint32_t)cl(env, 0xA4C0, 0, 0, 0, 0, 0, { game });
    const uint32_t id = rd32(env->p(pl), 0x10 + rd8(env->p(unit), 0x6d) * 4);
    auto st = [&](uint32_t n) { return (uint32_t)cl(env, 0xA568, 0, 0, 0, 0, 0, { id, 0x11, n }); };
    if (st(0) == 1 || ev != 0xfe) return;
    const uintptr_t d = env->p(rd32(q, 0x18));
    auto e5eb0 = [&](uint32_t n) { cl(env, 0x35EB0, n, 0, 0, 0, qh); };                          // EAX = n, ESI = q
    auto e5910 = [&]() { cl(env, 0x35910, arg, game, 0, 0, 0, { unit }); };                      // EAX = arg, ECX = game
    if (sub == 0x225) {
        e5eb0(2);
        wr8(q, 0x14, 0); wr8(q, 0xb, 1);
        env->iter(0xE6550, rd32(q, 4), 0);
        env->iter(0xE5370, rd32(q, 4), 0);
        e5910();
    } else if (sub == 0x234) {
        e5910();
        if (st(0) != 0) return;
        if ((uint32_t)cl(env, 0xE660, game, 0, 0, unit, 0, { 0x20626262 })) {
            cl(env, 0x366A0, 0x20626262, unit, 0, 0, game);                                       // EAX = tag, ECX = unit, ESI = game
            wr8(d, 0xc, 1);
            wr32(d, 0x10, game ? rd32(env->p(game), 0xc) : 0xffffffffu);
            wr32(q, 0xa8, env->addr(0xE51C0));
        }
        if (rd8(d, 0) == 1) {
            env->iter(0xE64C0, unit, arg);
            wr8(d, 0, 0);
            env->iter(0xE6470, unit, arg);
            env->iter(0xE5230, unit, arg);
        }
        wr8(q, 0x14, 0); wr8(q, 0xb, 0xd);
        cl(env, 0x33880, qh + 0x1c, 0, 0, 0, game ? rd32(env->p(game), 0xc) : 0xffffffffu);      // EAX = &q[0x1c], ESI = game id
        if (st(0xd) != 0 && rd8(q, 9) != 0) {
            cl(env, 0x33920, unit, 0, 0, 0, 0, { 0x11, 0xd });
            if (rd8(q, 0xc) != 5) {
                e5eb0(5);
                // IsBadCodePtr(q[0xf0]) is a host import; a bad pointer is the fatal-exit path and is not ported
                cl(env, rd32(q, 0xf0) - env->addr(0), 0, qh, 0, 0, 0);
            }
        }
    }
}
