// 1.13c D2Game+0xAE4D0: the stamina-potion state applier with every callee a seam on D2GameBatchAD860Env.
// The 1.13c game runs SKILLITEM_pSpell09_StaminaPotion (D2Game/SKILLS/SkillItem.cpp); this env-seamed copy is what the FN probe
// (src/core/fn/d2game_ae4d0.cpp) records against retail. Restored from the pre-sweep d2game_batch_ad860.cpp (git show 1b9121ac03^:...).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2game_batch_ad860_env.h"

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline int16_t  rd16(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
#define O(n, a, b, c, d, e) env->ord(n, (uintptr_t)(a), (uintptr_t)(b), (uintptr_t)(c), (uintptr_t)(d), (uintptr_t)(e))

// 1.13c D2Game+0xAE4D0   ecx = game, edx = unit, stack p3 (ret 0x14). The record the first ordinal returns is the one whose +0x9e/+0xa4
// arrays are walked, its +0xb0 word is the first 10428 call's third argument, the duration adds that call's result, and the new state
// is created on the game's +0x1c with the skill id.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_AE4D0(void* p1, int32_t* unit, void* p3, const D2GameBatchAD860Env* env)
{
    int32_t iVar1 = p3 ? (int32_t)rd32(p3, 4) : -1;
    uint32_t rec = O(10695, iVar1, 0, 0, 0, 0);
    if (!rec) return 0;
    const void* r = (const void*)(uintptr_t)rec;
    int32_t skill = rd16(r, 0x98);
    if (!(0 < skill && skill < (int32_t)rd32(env->dt, 0xc4))) return 0;
    const int32_t cnt = (int32_t)O(10428, unit, p3, rd32(r, 0xb0), 0, 0);
    if (cnt <= 0) return 0;
    env->subBEA30(skill, 0);
    int32_t base = 0;
    uint32_t sk = O(10871, unit, skill, 0, 0, 0);
    if (sk) base = (int32_t)O(10959, sk, 0, 0, 0, 0) - (int32_t)rd32(p1, 0xa8);
    uint32_t amt = rd32(p1, 0xa8) + base + cnt;
    if (!sk) {
        int32_t a, b;
        if (!unit) { b = -1; a = 6; } else { b = unit[3]; a = unit[0]; }
        sk = O(11013, rd32(p1, 0x1c), 2, 0, a, b);
        if (!sk) return 0;
        O(10785, sk, skill, 0, 0, 0);
        O(10945, unit, skill, 1, 0, 0);
        O(10911, sk, (uintptr_t)env->subBF7A0, 0, 0, 0);
        O(10807, unit, sk, 1, 0, 0);
    }
    if (amt) {
        O(11043, sk, amt, 0, 0, 0);
        env->subAEA80((uint32_t)(uintptr_t)p1, unit, 0xc, amt, 0, 0, 0);
    }
    for (int i = 0; i < 3; i++) {
        int32_t id = rd16(r, 0x9e + 2 * i);
        if (-1 < id && id < (int32_t)rd32(env->dt, 0xbd4) && env->sub210D0() != 0) {
            int32_t v = (int32_t)O(10428, unit, p3, rd32(r, 0xa4 + 4 * i), 0, 0);
            if (v != 0) {
                O(10188, sk, id, v, 0, 0);
                if (id == 0x1c && 0 < v) {
                    int32_t hi = (int32_t)O(10973, unit, 0xb, 0, 0, 0);
                    int32_t lo = (int32_t)O(10973, unit, 10, 0, 0, 0);
                    if (lo < hi) O(10887, unit, 10, hi + 0x100, 0, 0);
                }
            }
        }
    }
    return 1;
}
