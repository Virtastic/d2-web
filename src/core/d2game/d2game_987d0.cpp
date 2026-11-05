// Damage-range roll in 1/256 units: ESI = unit (plain ret). Without the owner record the min/max stats (0x15/0x16) are read and
// clamped to at least 1 and 2; with it, a unit whose mode query on its +0x60 word is 2 reads 0x17/0x18 instead (no clamp). Both are
// shifted by 8, min below 1 becomes 0x100, max not above min becomes min + 0x100. The unit's seed (+0x20 low, +0x24 high)
// advances once (low * 0x6AC690C5 + high, 64-bit) and the result is min + (low & (span - 1)) for a power-of-two span, else
// min + low % span. A rule over E (E::h = the callee call, E::rd32/wr32 = memory).
#ifndef D2GAME_RULE_987D0
#define D2GAME_RULE_987D0
#include <cstdint>

namespace g987d0 {
template <class E> uint32_t Roll(E& e, uint32_t u)
{
    int32_t a, b;
    if (!e.h(0x372C0, 0, 0, 0, 0, 0, u)) {
        a = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, u, 0x15, 0); if (a <= 0) a = 1;
        b = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, u, 0x16, 0); if (b <= 1) b = 2;
    } else {
        const bool alt = e.h(0xA7C0, 0, 0, 0, 0, 0, u, e.rd32(u + 0x60)) == 2;
        a = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, u, alt ? 0x17 : 0x15, 0);
        b = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, u, alt ? 0x18 : 0x16, 0);
    }
    a = (int32_t)((uint32_t)a << 8); b = (int32_t)((uint32_t)b << 8);
    if (a < 1) a = 0x100;
    if (b <= a) b = (int32_t)((uint32_t)a + 0x100);
    const int32_t d = (int32_t)((uint32_t)b - (uint32_t)a);
    if (d <= 0) return (uint32_t)a;
    const uint64_t v = (uint64_t)e.rd32(u + 0x20) * 0x6AC690C5ull + e.rd32(u + 0x24);
    e.wr32(u + 0x20, (uint32_t)v); e.wr32(u + 0x24, (uint32_t)(v >> 32));
    const uint32_t s = (uint32_t)d, lo = (uint32_t)v;
    return (uint32_t)a + ((s & (s - 1)) ? lo % s : (lo & (s - 1)));
}
}
#endif
