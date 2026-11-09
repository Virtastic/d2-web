// 1.13c D2Game+0xDEBA0, from the disassembly: EAX = unit A, EBX = unit B (seed at +0x20/+0x24,
// pointer to the skill holder at +0x60), one stack word (game), ret 4. Rolls use seed = lo * 0x6AC690C5 + hi (64-bit pair written
// back); a power-of-two modulus masks, otherwise an unsigned remainder. The result is EAX as retail leaves it on every path.
#include "d2game_deba0.h"
#include <cstring>
namespace {
uint32_t rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
void wr(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(a + o), &v, 4); }
uint32_t roll(uint32_t b, uint32_t mod) {
    const uint64_t s = (uint64_t)rd(b, 0x20) * 0x6ac690c5ull + rd(b, 0x24);
    wr(b, 0x20, (uint32_t)s); wr(b, 0x24, (uint32_t)(s >> 32));
    return (mod & (mod - 1)) == 0 ? ((mod - 1) & (uint32_t)s) : (uint32_t)s % mod;
}
}
uint32_t d2game_deba0_port(uint32_t eax, uint32_t ebx, uint32_t game, const D2GameDEBA0Env* e) {
    const uint32_t a = eax;
    if (a && rd(a, 0) == 0) {
        eax = e->sub372C0(a);
        if (eax) eax = e->sub12EE0(eax, a, game);
    }
    if (!ebx || rd(ebx, 0) != 0) return eax;
    const uint32_t holder = rd(ebx, 0x60);
    if (!holder) return eax;
    const int32_t n = (int32_t)e->weightCount;
    uint32_t slot[7] = {0}, total = 0;
    eax = holder;
    for (int32_t i = 0; i < n; ++i) {
        const uint32_t u = e->ordinal11139(holder, e->weightTable[i * 2]);
        eax = u;
        if (u && e->ordinal10744(u, 0x32)) {
            eax = e->weightTable[i * 2 + 1];
            slot[i] = u; total += eax;
        } else eax = 0;
    }
    if (n <= 0 || (int32_t)total <= 0) return eax;
    uint32_t idx = roll(ebx, (uint32_t)n);
    int32_t w = (int32_t)roll(ebx, total);
    int32_t cx = (int32_t)idx;
    for (;;) {
        if (slot[cx]) {
            const int32_t wt = (int32_t)e->weightTable[cx * 2 + 1];
            if (w < wt) return e->sub12EE0(slot[cx], ebx, game);
            w -= wt;
        }
        const int32_t nx = cx + 1;
        cx = nx % n;
        if (w < 0) return (uint32_t)(nx / n);
    }
}
