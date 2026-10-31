// 1.13c D2Game+0x13E60 -- second-layout item roll (EDI = item, stack word = arg, ret 4). With the #10926
// table and the class record present: #10536(item, roll1180(seed, table[0])). #10865 set -> prefix value
// v = class byte +0x112 * 33 / 100 (min 1); stat 0x48 = roll11d0(item seed +0x20, v>>1) + v>>1 (0 -> 1), stat
// 0x49 = v. Item type 0x2D scales stats 0x16/0x15/0x18/0x17 to 75% (floors 2/1/2/1) and, for #10711, 0x9F/0xA0
// through 0x13230 (floors 2/1, 75); type 0x32 scales stat 0x1F (floor 1). Every pass past the first guard ends
// in 0x13DE0(item, arg); the answer is 1 when a type matched. Retail pointers are 32-bit.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game13E60Env {
    uint32_t (*ordinal10926)();
    uint32_t (*ordinal10695)(uint32_t classId);
    uint32_t (*roll1180)(uint32_t seed, uint32_t nMax);     // 0x1180 (ECX seed, EAX nMax)
    uint32_t (*ordinal10411)(uint32_t item);
    void (*ordinal10536)(uint32_t item, uint32_t value);
    uint32_t (*ordinal10865)(uint32_t item);
    uint32_t (*roll11d0)(uint32_t seed, uint32_t nMax);     // 0x11D0 (ECX seed, ESI nMax)
    void (*ordinal10887)(uint32_t item, uint32_t stat, uint32_t value, uint32_t layer);
    uint32_t (*ordinal10744)(uint32_t item, uint32_t type);
    int32_t (*ordinal10587)(uint32_t item, uint32_t stat, uint32_t layer);
    uint32_t (*ordinal10711)(uint32_t item);
    int32_t (*ordinal10910)(uint32_t item, uint32_t stat, uint32_t layer);
    int32_t (*sub13230)(int32_t a, int32_t floor, int32_t pct);
    void (*sub13DE0)(uint32_t item, uint32_t arg);
};
static inline uint32_t g13e_rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
static inline int32_t g13e_pct75(int32_t v) { return (int32_t)((uint32_t)v * 0x4Bu) / 100; }
extern "C" int32_t d2_game_13E60(uint32_t item, uint32_t arg, const D2Game13E60Env* env)
{
    const uint32_t g = env->ordinal10926();
    const uint32_t cls = item ? g13e_rd32(item, 4) : 0xFFFFFFFFu;
    const uint32_t rec = env->ordinal10695(cls);
    if (g == 0 || rec == 0) return 0;
    env->ordinal10536(item, env->roll1180(env->ordinal10411(item), g13e_rd32(g, 0)));
    int32_t ret = 0;
    if (env->ordinal10865(item)) {
        const uint32_t rec2 = env->ordinal10695(item ? g13e_rd32(item, 4) : 0xFFFFFFFFu);
        if (rec2) {
            int32_t v = (int32_t)(uint32_t)(*(const uint8_t*)(uintptr_t)(rec2 + 0x112) * 0x21u) / 100;
            if (v < 1) v = 1;
            const uint32_t half = (uint32_t)v >> 1;
            uint32_t extra = env->roll11d0(item + 0x20, half) + half;
            if (extra == 0) extra = 1;
            env->ordinal10887(item, 0x48, extra, 0);
            env->ordinal10887(item, 0x49, (uint32_t)v, 0);
        }
    }
    if (env->ordinal10744(item, 0x2D)) {
        static const uint32_t stat[4] = { 0x16, 0x15, 0x18, 0x17 };
        static const int32_t floorv[4] = { 2, 1, 2, 1 };
        for (int i = 0; i < 4; ++i) {
            int32_t q = g13e_pct75(env->ordinal10587(item, stat[i], 0));
            if (q < floorv[i]) q = floorv[i];
            env->ordinal10887(item, stat[i], (uint32_t)q, 0);
        }
        if (env->ordinal10711(item)) {
            env->ordinal10887(item, 0x9F, (uint32_t)env->sub13230(env->ordinal10910(item, 0x9F, 0), 2, 0x4B), 0);
            env->ordinal10887(item, 0xA0, (uint32_t)env->sub13230(env->ordinal10910(item, 0xA0, 0), 1, 0x4B), 0);
        }
        ret = 1;
    } else if (env->ordinal10744(item, 0x32)) {
        int32_t q = g13e_pct75(env->ordinal10587(item, 0x1F, 0));
        if (q <= 0) q = 1;
        env->ordinal10887(item, 0x1F, (uint32_t)q, 0);
        ret = 1;
    }
    env->sub13DE0(item, arg);
    return ret;
}
