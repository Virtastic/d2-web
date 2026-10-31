// 1.13c D2Game+0x14210 -- table pick (stdcall unit, flag; ret 8). #10220 returns {+0 count, +4 element array (0x48
// bytes each), +8 / +0xC two cursors}. flag 0 walks [+8, +0xC), flag != 0 walks [+0xC, end of the array); every
// element #10110(unit, element) accepts is collected as {index, element} up to 0x1FF, the unit's item seed
// (#10411, 0x6AC690C5 step) picks one, and the answer is its index + 1. Empty answers keep what retail leaves
// in EAX: the walk's cursor pointer's high half (no cursor), or the high half of bytes * 0x38E38E39 (no
// element), else 0.
#include <cstdint>
#include <cstring>
struct D2Game14210Env { uint32_t (*ordinal10220)(); uint32_t (*ordinal10110)(uint32_t unit, uint32_t element); uint32_t (*ordinal10411)(uint32_t unit); };
static inline uint32_t g_rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
extern "C" uint32_t d2_game_14210(uint32_t unit, uint32_t flag, const D2Game14210Env* env)
{
    const uint32_t p = env->ordinal10220();
    const uint32_t cur = flag ? g_rd(p, 0xC) : g_rd(p, 8);
    if (!cur) return (flag ? p : cur) & 0xffff0000u;
    const uint32_t base = g_rd(p, 4);
    const int32_t span = flag ? (int32_t)(g_rd(p, 0) * 0x48u - g_rd(p, 0xC) + base) : (int32_t)(g_rd(p, 0xC) - g_rd(p, 8));
    const int32_t n = span / 0x48;
    const int32_t first = (int32_t)(cur - base) / 0x48;
    uint32_t pos[0x1FF];
    uint32_t got = 0, el = cur;
    for (int32_t j = 0; j < n; ++j, el += 0x48)
        if (env->ordinal10110(unit, el) && (int32_t)got < 0x1FF) pos[got++] = (uint32_t)(first + j);
    if (n <= 0) return (uint32_t)span * 0x38E38E39u & 0xffff0000u;
    if (!got) return 0;
    const uint32_t seed = env->ordinal10411(unit);
    uint32_t lo = g_rd(seed, 0), hi = g_rd(seed, 4);
    const uint64_t prod = (uint64_t)lo * 0x6AC690C5u + hi;
    const uint32_t nlo = (uint32_t)prod, nhi = (uint32_t)(prod >> 32);
    std::memcpy((uint8_t*)(uintptr_t)seed, &nlo, 4); std::memcpy((uint8_t*)(uintptr_t)seed + 4, &nhi, 4);
    const uint32_t r = (got & (got - 1)) ? nlo % got : (nlo & (got - 1));
    return pos[r] + 1;
}
