// 1.13c D2Game+0x140B0 -- pick an item-type slot for an item (stdcall item, arg; ret 8). #10295 gives {+0 slot
// count}; the count is forced to 4 when #10711(item) or the class record's byte +0x113 is set. Slots are drawn
// with the item seed (#10411, 0x6AC690C5 step; mask when the count is a power of two, modulo otherwise) until
// #11007(slot) yields a type that #11094(item, type) accepts; a refused slot is marked and the draw repeats
// while an unmarked slot remains (none left, or no type: 0). On success #10536(item, slot),
// #10996(1,0,item,type,0,0) and 0x13DE0(item, arg) run and the answer is 1.
#include <cstdint>
#include <cstring>
struct D2Game140B0Env {
    uint32_t (*ordinal10295)(); uint32_t (*ordinal10695)(uint32_t); uint32_t (*ordinal10711)(uint32_t);
    uint32_t (*ordinal10411)(uint32_t); uint32_t (*ordinal11007)(uint32_t); uint32_t (*ordinal11094)(uint32_t, uint32_t);
    void (*ordinal10536)(uint32_t, uint32_t); void (*ordinal10996)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    void (*sub13DE0)(uint32_t item, uint32_t arg);
};
static inline uint32_t g140_rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
extern "C" uint32_t d2_game_140B0(uint32_t item, uint32_t arg, const D2Game140B0Env* env)
{
    const uint32_t tbl = env->ordinal10295();
    const uint32_t rec = env->ordinal10695(item ? g140_rd(item, 4) : 0xFFFFFFFFu);
    if (!tbl || !rec) return 0;
    int32_t n = (int32_t)g140_rd(tbl, 0);
    uint8_t tried[64] = {};
    if (env->ordinal10711(item) || *(const uint8_t*)(uintptr_t)(rec + 0x113)) n = 4;
    for (;;) {
        uint32_t slot = 0;
        do {
            const uint32_t seed = env->ordinal10411(item);
            if (n > 0) {
                uint32_t lo = g140_rd(seed, 0), hi = g140_rd(seed, 4);
                const uint64_t prod = (uint64_t)lo * 0x6AC690C5u + hi;
                const uint32_t nlo = (uint32_t)prod, nhi = (uint32_t)(prod >> 32);
                std::memcpy((uint8_t*)(uintptr_t)seed, &nlo, 4); std::memcpy((uint8_t*)(uintptr_t)seed + 4, &nhi, 4);
                slot = ((uint32_t)n & ((uint32_t)n - 1)) ? nlo % (uint32_t)n : (nlo & ((uint32_t)n - 1));
            } else slot = 0;
        } while (tried[slot]);
        const uint32_t type = env->ordinal11007(slot);
        if (!type) return 0;
        if (env->ordinal11094(item, type)) {
            env->ordinal10536(item, slot);
            env->ordinal10996(1, 0, item, type, 0, 0);
            env->sub13DE0(item, arg);
            return 1;
        }
        tried[slot] = 1;
        int32_t i = 0;
        if (n <= 0) return 0;
        while (tried[i]) if (++i >= n) return 0;
    }
}
