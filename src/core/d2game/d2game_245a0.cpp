// D2Game+0x245A0 -- spawn a random-sized group of minions owned by a unit.
// eax = minCount, ebx = owner, stack (game, mode, maxCount) ret 0xc.
// Gate: an owner monster whose data byte +0x16 has bit 4 spawns nothing. cls = 0x21F60(eax owner) must lie in
// [0, *(*0x6FD1829C)+0xa80) (the monstats row count, passed in). count = roll(owner+0x20, max-min+1) + min.
// Per minion: mode != 0 -> 0xEF570(ecx 1, edx owner, eax cls; game, mode, 3, 0x40), else 0xEDDB0(ecx 1, edx owner,
// eax cls; game, 3, 0x40). A made unit gets 0x23060(ecx new, eax owner; 0); a monster's AI link (data+0x28) gets
// +0x2c = owner id (or -1), +0x30 = 1, +0x28 = game; 0x20CF0(eax owner; game); then link +0xc = owner id, +0x10 =
// owner type (or -1 / 1 for no owner) and data byte +0x16 |= 0x10. A null link is written through as in retail.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game245A0Env {
    int32_t  (*sub21F60)(uint32_t owner);
    uint32_t monStatsCount;                                     // *(*0x6FD1829C + 0xa80)
    int32_t  (*roll)(uint32_t seed, int32_t range);             // 0x11D0 (ecx seed, esi range)
    uint32_t (*subEF570)(uint32_t ecx, uint32_t edx, uint32_t eax, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
    uint32_t (*subEDDB0)(uint32_t ecx, uint32_t edx, uint32_t eax, uint32_t s1, uint32_t s2, uint32_t s3);
    void     (*sub23060)(uint32_t ecx, uint32_t eax, uint32_t s1);
    void     (*sub20CF0)(uint32_t eax, uint32_t s1);
};

static inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }

// 1.13c D2Game+0x245A0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_245a0(int32_t minCount, uint32_t owner, uint32_t game, uint32_t mode,
                                                       int32_t maxCount, const D2Game245A0Env* env)
{
    if (owner != 0 && rd32(owner) == 1) {
        const uint32_t d = rd32(owner + 0x14);
        if (d != 0 && (*(const uint8_t*)(uintptr_t)(d + 0x16) & 4)) return;
    }
    const int32_t cls = env->sub21F60(owner);
    if (cls < 0 || cls >= (int32_t)env->monStatsCount) return;
    int32_t count = env->roll(owner + 0x20, maxCount - minCount + 1) + minCount;
    if (count <= 0) return;
    do {
        const uint32_t u = mode != 0 ? env->subEF570(1, owner, (uint32_t)cls, game, mode, 3, 0x40)
                                     : env->subEDDB0(1, owner, (uint32_t)cls, game, 3, 0x40);
        if (u == 0) continue;
        env->sub23060(u, owner, 0);
        const uint32_t ownerId = owner ? rd32(owner + 0xc) : 0xffffffffu;
        if (rd32(u) == 1) {
            const uint32_t d = rd32(u + 0x14);
            const uint32_t link = d ? rd32(d + 0x28) : 0;
            wr32(link + 0x2c, ownerId); wr32(link + 0x30, 1); wr32(link + 0x28, game);
        }
        env->sub20CF0(owner, game);
        if (rd32(u) == 1) {
            const uint32_t d = rd32(u + 0x14);
            const uint32_t link = d ? rd32(d + 0x28) : 0;
            if (owner) { wr32(link + 0xc, rd32(owner + 0xc)); wr32(link + 0x10, rd32(owner)); }
            else       { wr32(link + 0xc, 0xffffffffu); wr32(link + 0x10, 1); }
        }
        if (rd32(u) == 1) {
            const uint32_t d = rd32(u + 0x14);
            if (d) *(uint8_t*)(uintptr_t)(d + 0x16) |= 0x10;
        }
    } while (--count != 0);
}
