// 1.13c D2Game+0xE9600: a3q? quest event callback. ECX = q (quest record), EDX = msg (event block: +0 unit, +0xC game, +0x14 event, +0x18 sub-event).
#include <emscripten.h>
#include "d2game_s3a_env.h"
using namespace s3a;
// 1.13c D2Game+0xE9600
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E9600(uintptr_t q, uintptr_t msg, const D2GameS3aEnv* env)
{
    const uint16_t sub = rd16(msg, 0x18), ev = rd16(msg, 0x14);
    const uint32_t unit = rd32(msg, 0), game = rd32(msg, 0xc), qh = env->h(q);
    const uint32_t pl = (uint32_t)cl(env, 0xA4C0, 0, 0, 0, 0, 0, { game });
    const uint32_t id = rd32(env->p(pl), 0x10 + rd8(env->p(unit), 0x6d) * 4);
    auto st = [&](uint32_t n) { return (uint32_t)cl(env, 0xA568, 0, 0, 0, 0, 0, { id, 0x13, n }); };
    auto set = [&](uint32_t n) { cl(env, 0xA57A, 0, 0, 0, 0, 0, { id, 0x13, n }); };
    if (st(0) == 1) return;
    auto e5eb0 = [&](uint32_t n) { cl(env, 0x35EB0, n, 0, 0, 0, qh); };                                  // EAX = n, ESI = q
    auto e66a0 = [&](uint32_t tag) { cl(env, 0x366A0, tag, unit, 0, 0, game); };                          // EAX = tag, ECX = unit, ESI = game
    auto e660 = [&](uint32_t tag) { return (uint32_t)cl(env, 0xE660, game, 0, 0, unit, 0, { tag }); };   // EAX = game, EBX = unit
    auto e3880 = [&]() { cl(env, 0x33880, qh + 0x1c, 0, 0, 0, game ? rd32(env->p(game), 0xc) : 0xffffffffu); };   // EAX = &q[0x1c], ESI = game id
    if (ev == 0xff) {
        if (sub == 0x24b) {
            if (e660(0x20333367) && st(5) != 0 && st(8) == 0) {
                const uintptr_t d = env->p(rd32(q, 0x18));
                wr8(d, 7, 1);
                e66a0(0x20333367);
                wr32(d, 0x1c, rd32(d, 0x1c) - 1);
                set(6);
                if ((uint16_t)cl(env, 0xDFA60, game, 0, 0, 0, 0) != 0xffff) cl(env, 0x9C160, 0, 0, 0, 0, rd32(q, 4), { env->addr(0xE41D0) });
                env->iter(0xE4280, rd32(q, 4), 0);
            }
            if (rd8(q, 9) != 0) cl(env, 0x33920, unit, 0, 0, 0, 0, { 0x13, 0xd });
            wr8(q, 0x14, 0); wr8(q, 0xb, 0xd);
            if (rd8(q, 9) == 0 || rd8(q, 0xc) == 5) return;
            e5eb0(5);
            // IsBadCodePtr(q[0xf0]) is a host import; a bad pointer is the fatal-exit path and is not ported
            cl(env, rd32(q, 0xf0) - env->addr(0), 0, qh, 0, 0, 0);
        } else if (sub == 0x251) {
            if (st(6) == 0 || st(8) != 0) return;
            set(8);
            const uint8_t cls = rd8(env->p(unit), 0x6d);
            cl(env, 0x36470, cls == 1 ? 0x23 : cls == 2 ? 0x4b : 0x15, 0x206e6972, game, 0, 0, { unit, 6, 1 });
            if (st(7) == 0) return;
            set(0xd); set(0);
            e3880();
        }
    } else if (ev == 0xfd) {
        if (sub == 0x23b) { e5eb0(2); wr8(env->p(rd32(q, 0x18)), 1, 1); }
    } else if (ev == 0xfc) {
        if (sub != 0x24d || st(7) != 0) return;
        set(7);
        cl(env, 0xC1030, 0, 0, 0, 0, 0, { rd32(q, 4), game, 0xfc });
        if (st(8) == 0) return;
        set(0xd); set(0);
        e3880();
    }
}
