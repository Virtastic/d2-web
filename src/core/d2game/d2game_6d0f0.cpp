// D2Game+0x6D0F0 -- a player takes a town portal / red portal (ecx = S: +0 game, +4 portal, +8 player, +0x10 kind).
// Always returns 0. Refusals ("fail") send 0xDFFD0(esi player; 0x13, player). The order of the gates, the 5 s
// re-entry timer, the party-id check, the quest-flag gate for kind 0x3b, the destination (the paired portal or the
// level's warp point via #aba4), the move (0xE19C0), the client notices (0x78550, 0x8C280), the paired-portal
// close and the final 0x4b+game[0xa8] state item are described in the private spec.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game6D0F0Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*a6a6)(uint32_t p);
    uint32_t (*a4c0)(uint32_t unit);                   // player data
    uint32_t (*sub37AC0)(uint32_t eax, uint32_t ecx);
    uint32_t (*b168)(uint32_t portal);                 // owner record (+4 owner id)
    uint32_t (*getTickCount)();
    void     (*subDFFD0)(uint32_t esi, uint32_t s1, uint32_t s2);
    uint16_t (*subDFA60)(uint32_t eax);
    uint32_t (*subE1570)(uint32_t game, uint32_t portal);
    uint32_t (*a478)(uint32_t unit);
    uint32_t (*sub1020)(uint32_t eax, uint32_t game);
    uint32_t (*b144)(uint32_t ecx);
    uint32_t (*sub37440)(uint32_t eax);
    uint32_t (*a568)(uint32_t a, uint32_t v, uint32_t z);
    uint8_t  (*a56e)(uint32_t level);
    uint32_t (*aba4)(uint32_t act, uint32_t level, uint32_t z, uint32_t outX, uint32_t outY, uint32_t three);
    int32_t  (*sub1100)(uint32_t unit);
    int32_t  (*sub1130)(uint32_t unit);
    uint8_t  (*aee0)(uint32_t unit);
    void     (*aec8)(uint32_t unit, uint32_t v);
    uint32_t (*a808)(uint32_t room);
    uint32_t (*a556)(uint32_t room);
    void     (*sub34860)(uint32_t eax, uint32_t edx, uint32_t ecx, uint32_t s1);
    uint32_t (*a71e)(uint32_t unit);
    uint32_t (*a694)(uint32_t room, uint32_t pt, uint32_t a3, uint32_t a4, uint32_t a5);
    uint32_t (*subE19C0)(uint32_t ecx, uint32_t eax, uint32_t edx, uint32_t esi, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
    void     (*sub78550)(uint32_t ecx, uint32_t edx, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5);
    void     (*sub8C280)(uint32_t cl, uint32_t eax, uint32_t edx, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6);
    void     (*a44e)(uint32_t room, uint32_t a2, uint32_t id);
    void     (*subE20A0)(uint32_t ecx, uint32_t edx);
    void     (*a51a)(uint32_t room, uint32_t v);
    void     (*sub56740)(uint32_t eax, uint32_t s1);
    uint32_t (*a520)(uint32_t cls);                     // object txt row
    void     (*sub8EA80)(uint32_t ecx, uint32_t edx, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5);
    uint32_t (*a6a0)(uint32_t pool, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5);
    void     (*a3ca)(uint32_t unit, uint32_t stat, uint32_t v);
    void     (*a5b6)(uint32_t it, uint32_t stat);
    void     (*a75a)(uint32_t it, uint32_t cb);
    void     (*a60a)(uint32_t unit, uint32_t it, uint32_t v);
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline uint8_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
static inline uint32_t uid(uint32_t u) { return u ? rd32(u + 0xc) : 0xffffffffu; }
static inline uint32_t utype(uint32_t u) { return u ? rd32(u) : 6u; }

// 1.13c D2Game+0x6D0F0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_6d0f0(uint32_t S, const D2Game6D0F0Env* env)
{
    const uint32_t game = rd32(S), portal = rd32(S + 4);
    uint32_t pl = rd32(S + 8);
    if (pl != 0 && rd32(pl) == 0) {
        if (rd8(pl + 0x6c) != 0) return 0;
        if (env->a6a6(pl + 0x60) != 0) return 0;
        if (rd32(env->a4c0(pl) + 0x4c) != 0) return 0;
    }
    if (env->sub37AC0(pl, game) != 0) {
        const uint32_t own = env->b168(portal);
        if (uid(pl) != (own ? rd32(own + 4) : 0xffffffffu)) return 0;
    }
    uint32_t stamp = 0;
    if (const uint32_t pd = env->a4c0(pl)) {
        stamp = rd32(pd + 0x160);
    } else if (pl != 0) {
        for (uint32_t n = rd32(env->a4c0(pl) + 0x38); n != 0; n = rd32(n + 0x10))
            if (rd32(n) == 0xffffffffu) { stamp = rd32(n + 8); break; }
    }
    if (env->getTickCount() < stamp + 5000u) { env->subDFFD0(pl, 0x13, pl); return 0; }
    {
        const uint32_t own = env->b168(portal);
        const uint32_t t = own ? rd32(own + 4) : 0xffffffffu;
        if (t != 0xffffffffu && t != uid(pl)) {
            if (env->subDFA60(pl) == 0xffff) { env->subDFFD0(pl, 0x13, pl); return 0; }
            if (game == 0) { env->fatal(0x2d3); return 0; }
            for (uint32_t p = rd32(game + 0x1120 + (t & 0x7f) * 4); p != 0; p = rd32(p + 0xe4)) {
                if (rd32(p + 0xc) == t) {
                    if (env->subDFA60(p) != env->subDFA60(pl)) { env->subDFFD0(pl, 0x13, pl); return 0; }
                    break;
                }
            }
        }
    }
    if (pl == 0 || rd32(pl) != 0) { env->fatal(0xed8); return 0; }
    uint32_t pair = env->subE1570(game, portal);
    if (pair != 0 && rd32(pair) != 2) pair = 0;

    if (env->a478(portal) != 0) {
        const uint32_t ebp = env->sub1020(pl, game);
        const uint32_t esi = ebp ? env->b144(rd8(rd32(portal + 0x14)) + 4) : 0;
        if (ebp == 0 || esi == 0) { env->subDFFD0(pl, 0x13, pl); return 0; }
        if (rd32(S + 0x10) == 0x3b) {
            const uint32_t a = env->sub37440(pl);
            if (a != uid(portal)) {
                if (pair != 0 && a != rd32(pair + 0xc)) {
                    const int32_t v = (int32_t)(rd32(game + 0x70) == 0 ? rd32(esi) : rd32(esi + 4));
                    if (v > 0 && env->a568(ebp, (uint32_t)v, 0) == 0) { env->subDFFD0(pl, 0x13, pl); return 0; }
                }
            }
        }
    }
    int32_t pt[2];
    uint32_t room;
    if (pair != 0) {
        pt[0] = env->sub1100(pair);
        pt[1] = env->sub1130(pair);
        room = env->a478(pair);
    } else {
        const uint32_t lvl = rd8(rd32(portal + 0x14) + 4);
        const uint8_t act = env->a56e(lvl);
        room = env->aba4(rd32(game + 0xbc + act * 4u), lvl, 0,
                         (uint32_t)(uintptr_t)&pt[0], (uint32_t)(uintptr_t)&pt[1], 3);
    }
    // retail keeps stale stack bits above the low byte; only the low byte is defined
    env->aec8(portal, (uint8_t)(env->aee0(portal) | 5));
    const int32_t x = pt[0], y = pt[1];
    if (env->a808(env->a478(pl)) != 0)
        env->sub34860(game, env->a556(room), env->a556(env->a478(pl)), pl);
    const uint32_t room2 = env->a694(room, (uint32_t)(uintptr_t)pt, env->a71e(pl), 0x1c09, 0);
    if (room2 == 0) return 0;
    if (env->subE19C0((uint32_t)x, (uint32_t)y, room2, game, game, pl, 0, 0) == 0) { env->fatal(0xf52); return 0; }
    const uint32_t client = (rd32(pl) == 0) ? rd32(env->a4c0(pl) + 0x9c) : 0;
    env->subDFFD0(pl, 8, 0);
    env->sub78550(game, pl, 0, 2, (uint32_t)(x + 5), (uint32_t)(y + 5), 0);
    env->sub8C280(0xd, utype(pl), uid(pl), client, 1, (uint32_t)(x + 5), (uint32_t)(y + 5), 0, 0);
    const uint32_t cls = portal ? rd32(portal + 4) : 0xffffffffu;
    if (pair == 0 || (uint16_t)cls == 0x3c) {
        if (portal != 0 && rd32(portal + 0x10) == 1) {
            const uint32_t row = env->a520(cls & 0xffff);
            env->sub8EA80(game, portal, 1, (rd32(row + 0xdc) >> 8) + 1 + rd32(game + 0xa8), 0, 0, 0);
        }
    } else if (env->sub37440(pl) == rd32(pair + 0xc)) {
        uint32_t r = env->a478(portal);
        env->a44e(r, 2, uid(portal));
        env->subE20A0(game, portal);
        env->a51a(r, 1);
        env->sub56740(pair, game);
        r = env->a478(pair);
        env->a44e(r, 2, rd32(pair + 0xc));
        env->subE20A0(game, pair);
        env->a51a(r, 1);
    }
    const uint32_t st = rd32(game + 0xa8) + 0x4b;
    const uint32_t it = env->a6a0(rd32(game + 0x1c), 2, st, 0, utype(pl));
    if (it != 0) {
        env->sub8EA80(game, pl, 0xc, st, 0, 0, 0);
        env->a3ca(pl, 0x66, 1);
        env->a5b6(it, 0x66);
        env->a75a(it, 0x6fcbf7a0u);
        env->a60a(pl, it, 1);
    }
    return 0;
}
