// 1.13c D2Game+0xE8B60: a3q4 quest event callback (the q+0xcc slot). ECX = q (quest record), EDX = msg (event block).
// msg: +0 unit, +8 arg, +0xC game, +0x14 event, +0x18 sub-event. Register inputs of each callee are in the comments (retail order).
#include <emscripten.h>
#include "d2game_s3a_env.h"
using namespace s3a;
// 1.13c D2Game+0xE8B60
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E8B60(uintptr_t q, uintptr_t msg, const D2GameS3aEnv* env)
{
    const uint16_t sub = rd16(msg, 0x18), ev = rd16(msg, 0x14);
    const uint32_t unit = rd32(msg, 0), game = rd32(msg, 0xc), qh = env->h(q);
    const uint32_t pl = (uint32_t)cl(env, 0xA4C0, 0, 0, 0, 0, 0, { game });
    const uint32_t id = rd32(env->p(pl), 0x10 + rd8(env->p(unit), 0x6d) * 4);
    const uint32_t cb = env->addr(0xE6BD0);
    auto st = [&](uint32_t n) { return (uint32_t)cl(env, 0xA568, 0, 0, 0, 0, 0, { id, 0x14, n }); };
    auto set = [&](uint32_t n) { cl(env, 0xA57A, 0, 0, 0, 0, 0, { id, 0x14, n }); };
    if (st(0) == 1) return;
    const uintptr_t d = env->p(rd32(q, 0x18));
    auto e5eb0 = [&](uint32_t n) { cl(env, 0x35EB0, n, 0, 0, 0, qh); };                              // EAX = n, ESI = q
    auto e6b90 = [&]() { cl(env, 0xE6B90, game, 0, 0, 0, unit); };                                   // EAX = game, ESI = unit
    auto e5910 = [&]() { cl(env, 0x35910, rd32(msg, 8), game, 0, 0, 0, { unit }); };                 // EAX = arg, ECX = game
    auto e66a0 = [&](uint32_t tag) { cl(env, 0x366A0, tag, unit, 0, 0, game); };                     // EAX = tag, ECX = unit, ESI = game
    auto e6470 = [&](uint32_t tag) { return cl(env, 0x36470, 0, tag, game, 0, 0, { unit, 2, 1 }); }; // ECX = tag, EDX = game
    auto e660 = [&](uint32_t tag) { return (uint32_t)cl(env, 0xE660, game, 0, 0, unit, 0, { tag }); }; // EAX = game, EBX = unit
    if (ev == 0xf5) {
        if (sub == 0x20f) {
            set(2);
            if (rd8(q, 0xc) == 1) { e5eb0(2); wr8(d, 9, 1); wr32(q, 0xa8, cb); }
            e5910(); wr32(d, 0x18, 2); e6b90();
        } else if (sub == 0x213) {
            set(4); wr8(d, 10, 1); e5910(); wr32(d, 0x18, 4); e6b90();
        }
    } else if (ev == 0xfe) {
        if (sub == 0x216) {
            if (!e660(0x20343367)) return;
            if (st(1) != 0 || st(0) != 0) return;
            e66a0(0x20343367);
            wr32(d, 0x10, rd32(d, 0x10) - 1);
            if (rd8(q, 9) != 0 && rd8(q, 0xc) != 4) e5eb0(4);
            set(1); wr8(d, 8, 1); wr32(q, 0xa8, cb); wr32(d, 0x18, 1); e6b90();
        } else if (sub == 0x21a && st(0) == 0) {
            if (st(1) != 0) {
                cl(env, 0xA586, 0, 0, 0, 0, 0, { id, 0x14, 1 });
                set(0); set(0xd);
                cl(env, 0xA574, 0, 0, 0, 0, 0, { id, rd32(q, 0xe0) });
                set(5);
                if (rd8(q, 9) != 0) e5eb0(5);
                e6470(0x207a7978);
                cl(env, 0x34C00, game, 0, 0, 0, 0, { unit });
                cl(env, 0x33880, qh + 0x1c, 0, 0, 0, game ? rd32(env->p(game), 0xc) : 0xffffffffu);   // EAX = &q[0x1c], ESI = game id
            }
            if (st(0xd) != 0) {
                cl(env, 0x33920, unit, 0, 0, 0, 0, { 0x14, 0xd });
                // IsBadCodePtr(q[0xf0]) is a host import; a bad pointer is the fatal-exit path and is not ported
                cl(env, rd32(q, 0xf0) - env->addr(0), 0, qh, 0, 0, 0);
            }
        }
    } else if (ev == 0x108 && sub == 0x211) {
        if (!e660(0x2034336a)) return;
        wr32(q, 0xa8, cb);
        e66a0(0x2034336a);
        wr32(d, 0x10, rd32(d, 0x10) - 1);
        if (e6470(0x20343367)) { wr32(d, 0x10, rd32(d, 0x10) + 1); e5eb0(3); wr8(d, 0xb, 1); wr32(q, 0xa8, cb); }
    }
}
