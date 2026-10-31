// D2Game+0x36890 -- quest flag updates when a player interacts with one of three objects.
// ecx = player, eax = target, stack game ret 4. flags = PlayerData(player)->[0x10 + difficulty*4]; only a target of
// type 1 is handled, switched on its class (+4): 0x9b, 0xd2, 0x16f. #a568(flags, quest, bit) tests,
// #a57a(flags, quest, bit) sets. client = player of type 0 ? *(PlayerData(player)+0x9c) : 0. The per-player byte gate
// is PlayerData(player)+0x4c (set to 1 the first time).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game36890Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*a4c0)(uint32_t unit);                              // #10920 PlayerData
    uint32_t (*a568)(uint32_t flags, uint32_t quest, uint32_t bit);
    void     (*a57a)(uint32_t flags, uint32_t quest, uint32_t bit);
    void     (*sub35C20)(uint8_t cl, uint32_t eax, uint32_t edx);
    void     (*sub34C00)(uint32_t eax, uint32_t s1);
    void     (*sub366A0)(uint32_t eax, uint32_t ecx);
    void     (*sub683C0)(uint32_t eax, uint32_t s1);
    void     (*sub8AAC0)(uint8_t al, uint32_t s1);
    void     (*sub8ACD0)(uint32_t eax, uint32_t s1);
    uint32_t (*isBadCodePtr)(uint32_t fn);
    void     (*call)(uint32_t fn, uint32_t ecx, uint32_t edx);
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
static uint32_t clientOf(uint32_t pl, const D2Game36890Env* env) { return (pl != 0 && rd32(pl) == 0) ? rd32(env->a4c0(pl) + 0x9c) : 0; }

// 1.13c D2Game+0x36890
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_36890(uint32_t player, uint32_t target, uint32_t game, const D2Game36890Env* env)
{
    const uint32_t flags = rd32(env->a4c0(player) + 0x10 + *(const uint8_t*)(uintptr_t)(game + 0x6d) * 4);
    if (target == 0 || rd32(target) != 1) return;
    const uint32_t pd = env->a4c0(player);
    switch (rd32(target + 4)) {
    case 0x9b: {
        if (env->a568(flags, 7, 0) == 0) {
            env->a57a(flags, 7, 0);
            env->a57a(flags, 7, 0xd);
            env->sub34C00(player, game);
            const uint32_t c = clientOf(player, env);
            if (rd32(pd + 0x4c) != 1) { wr32(pd + 0x4c, 1); env->sub8AAC0(2, c); }
            env->sub35C20(0, player, game);
        }
        if (env->a568(flags, 6, 0xd) == 1 && env->a568(flags, 6, 0) == 1) {
            const uint32_t list = rd32(game + 0x10f4);
            uint32_t n = list ? rd32(list) : 0;
            if (list != 0 && n != 0) {
                while (n != 0 && rd32(n) != 6) n = rd32(n + 0xf4);
                if (n != 0 && *(const uint8_t*)(uintptr_t)(n + 9) != 0) {
                    const uint32_t fn = rd32(n + 0xac);
                    uint32_t block[7] = { game, 3, 0, player, 0, 1, 0x28 };
                    if (fn != 0) {
                        if (env->isBadCodePtr(fn)) { env->fatal(0xaee); return; }
                        env->call(rd32(n + 0xac), n, (uint32_t)(uintptr_t)block);
                    }
                }
            }
        }
        env->sub683C0(player, game);
        return;
    }
    case 0xd2: {
        if (env->a568(flags, 0xf, 0) != 0) return;
        if (env->a568(flags, 0xa, 0) == 0) {
            env->a57a(flags, 0xa, 0);
            env->a57a(flags, 0xa, 0xd);
            env->sub366A0(0x2066736d, game);   // 'msf '
            env->sub366A0(0x20706976, game);   // 'vip '
        }
        env->a57a(flags, 0xf, 0);
        env->a57a(flags, 0xf, 0xd);
        env->sub35C20(1, player, game);
        env->sub34C00(player, game);
        const uint32_t c = clientOf(player, env);
        if (rd32(pd + 0x4c) == 1) return;
        wr32(pd + 0x4c, 1);
        env->sub8AAC0(3, c);
        return;
    }
    case 0x16f: {
        if (env->a568(flags, 0x1c, 0) != 0 || rd32(game + 0x70) == 0) return;
        env->a57a(flags, 0x1c, 0);
        env->a57a(flags, 0x1c, 0xd);
        env->sub35C20(1, player, game);
        env->sub34C00(player, game);
        const uint32_t c = clientOf(player, env);
        if (rd32(pd + 0x4c) == 1) return;
        wr32(pd + 0x4c, 1);
        uint8_t pkt[6] = { 0x5d, 0x17, 0x02, 0, 0, 0 };
        env->sub8ACD0((uint32_t)(uintptr_t)pkt, c);
        env->sub8AAC0(5, c);
        return;
    }
    default: return;
    }
}
