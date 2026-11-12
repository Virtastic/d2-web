// 1.13c D2Common+0x52480 (#10056): refresh a passive skill's state stat list. Retail-layout (32-bit) reader over the
// data tables image with the eleven callees as seams. The skill row (stride 0x23C) must carry a passive state
// (+0x94) inside the states count (+0xC4). With no skill of that id (0x4E480 returns 0) the state's list is looked
// up under the unit's +0x5C list (flag bit 31), expired (0x3A3B0) and freed (0x3A9F0) once its own bit 31 is clear.
// With an aura state (+0x80) that is set (0x33CD0), 0x522C0 runs with level 0. Otherwise the level (0x505C0) gets its
// list (0x522C0); when the level stat 0x15F differs, the five passive stats (+0x98 shorts, +0xA4 calcs) are written
// (layer = +0x96 when positive, else 0), then stats 0x15E (skill) and 0x15F (level) and the state's gfx toggle.
#include <cstdint>
#include <cstring>

struct D2Common52480Env {
    uint32_t (*unitSkill)(uint32_t); uint32_t (*getStateList)(uint32_t, int32_t); void (*expire)(uint32_t); void (*freeList)(uint32_t);
    uint32_t (*hasState)(uint32_t, int32_t); uint32_t (*skillLevel)(uint32_t, uint32_t, uint32_t); uint32_t (*stateList)(uint32_t, int32_t, uint32_t);
    uint32_t (*listStat)(uint32_t, uint32_t, uint32_t); int32_t (*formula)(uint32_t, uint32_t, int32_t, uint32_t);
    void (*setStat)(uint32_t, int32_t, int32_t, uint32_t); void (*toggle)(uint32_t, int32_t, uint32_t); };

namespace {
inline uint32_t Rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline int32_t Rd16(uint32_t a) { int16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
}

extern "C" void D2Common_52480_Raw(uint32_t dt, uint32_t unit, int32_t skillId, const D2Common52480Env* env)
{
    if (skillId < 0 || skillId >= (int32_t)Rd32(dt + 0xBA0)) return;
    const uint32_t row = (uint32_t)skillId * 0x23C + Rd32(dt + 0xB98);
    if (!row) return;
    const int32_t state = Rd16(row + 0x94);
    if (state <= 0 || state >= (int32_t)Rd32(dt + 0xC4)) return;
    const uint32_t skill = env->unitSkill(unit);
    if (!skill) {
        const uint32_t ex = Rd32(unit + 0x5C);
        if (!ex || !(Rd32(ex + 0x10) & 0x80000000u)) return;
        const uint32_t list = env->getStateList(ex, state);
        if (!list) return;
        env->expire(list);
        if (Rd32(list + 0x10) & 0x80000000u) return;
        env->freeList(list);
        return;
    }
    const int32_t aura = Rd16(row + 0x80);
    if (aura > 0 && env->hasState(unit, aura)) { env->stateList(unit, state, 0); return; }
    const uint32_t level = env->skillLevel(unit, skill, 1);
    const uint32_t list = env->stateList(unit, state, level);
    if (!list || env->listStat(list, 0x15F, 0) == level) return;
    for (int32_t i = 0; i < 5; ++i) {
        const int32_t stat = Rd16(row + 0x98 + 2 * i);
        if (stat < 0 || stat >= (int32_t)Rd32(dt + 0xBD4)) break;
        const int32_t v = env->formula(unit, Rd32(row + 0xA4 + 4 * i), skillId, level);
        const int32_t layer = Rd16(row + 0x96);
        env->setStat(list, stat, v, layer > 0 ? (uint32_t)(uint16_t)layer : 0u);
    }
    env->setStat(list, 0x15E, skillId, 0);
    env->setStat(list, 0x15F, (int32_t)level, 0);
    env->toggle(unit, state, 1);
}
