// 1.13c D2Game+0xAF370: the env-seamed copy the FN probe (src/core/fn/d2game_af370.cpp) records against retail.
// Restored from the pre-sweep d2game_batch_ad860.cpp (git show 1b9121ac03^:...).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2game_batch_ad860_env.h"

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline int16_t  rd16(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
#define O(n, a, b, c, d, e) env->ord(n, (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c), (uintptr_t)(d), (uintptr_t)(e))

// 1.13c D2Game+0xAF370   p1 is the ECX pass-through for 0xAE4D0 (Ghidra's iStack_8 slot); all five stack words
// (p3, s1..s4) are forwarded to it. The result is 1 when either state id is in range or 0xAE4D0 succeeded.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_AF370(uint32_t p1, int32_t* unit, void* p3, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, const D2GameBatchAD860Env* env)
{
    uint32_t rec = O(10695, p3 ? rd32(p3, 4) : 0xffffffffu, 0, 0, 0, 0);
    if (!rec) return 0;
    uint32_t ret = 0;
    for (int i = 0; i < 2; i++) {
        int32_t id = rd16((void*)(uintptr_t)rec, 0x9a + 2 * i);
        if (0 < id && id < (int32_t)rd32(env->dt, 0xc4)) {
            uint32_t sk = O(10871, unit, id, 0, 0, 0);
            if (sk) { O(11111, unit, sk, 0, 0, 0); O(11108, sk, 0, 0, 0, 0); }
            if (O(10494, unit, id, 0, 0, 0)) O(10945, unit, id, 0, 0, 0);
            ret = 1;
        }
    }
    if (env->subCE4D0(p1, unit, p3, s1, s2, s3, s4)) ret = 1;
    return ret;
}
