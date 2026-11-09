// D2Game+0x27A10: the Frenzy damage roll. EAX = skill id, stack: game, unit, target, level (ret 0x10). Needs a skills.txt row for the id and a
// target, else returns 0. The unit's target is set, a damage block (28 dwords, zeroed) takes the to-hit result flags; on a hit: flags |= row
// word +0x12E, hit flags |= row dword +0x130, hit class = row +0x134 when nonzero, the elemental-damage percentage (calc +0x138), and, when
// the row has an element type (+0x1DC), the conversion percentage (calc +0x144; the type is kept only when it is > 0), then the elemental roll
// (0x9F210). The combat entry takes the source damage byte (row +0x1A5, 0x80 when 0), the durability drain runs, and the unit's used skill (when
// any) gets param 1 = the hit bit. Returns 1 once past the guard. 1.13c delta over the 1.10f port: the guard order (row, then target) is the same.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2game_27a10_env.h"

static inline uint32_t rd32(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
static inline void wr32(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }

// 1.13c D2Game+0x27A10
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2game_27a10_env(int32_t id, void* game, void* unit, void* target, int32_t level, const D2Game27a10Env* env)
{
    if (id < 0) return 0;
    const uint8_t* rec = env->skillRecord(id);
    if (!rec || !target) return 0;
    env->setTarget(unit, target);
    uint8_t dmg[0x70] = {};
    const uint32_t flags = env->resultFlags(game, unit, target, env->toHit(unit, id, level));
    uint16_t rf = (uint16_t)flags;
    std::memcpy(dmg + 4, &rf, 2);
    if (flags & 1) {
        uint16_t w; std::memcpy(&w, rec + 0x12e, 2); rf |= w;
        std::memcpy(dmg + 4, &rf, 2);
        wr32(dmg, 0, rd32(dmg, 0) | rd32(rec, 0x130));
        if (rd32(rec, 0x134)) wr32(dmg, 0x60, rd32(rec, 0x134));
        wr32(dmg, 0xc, (uint32_t)env->formula(unit, (int32_t)rd32(rec, 0x138), id, level));
        if (rec[0x1dc]) {
            const int32_t conv = env->formula(unit, (int32_t)rd32(rec, 0x144), id, level);
            wr32(dmg, 0x68, (uint32_t)conv);
            if (conv > 0) dmg[0x65] = rec[0x1dc];
        }
        env->rollElemental(unit, dmg, id, level);
    }
    env->allocCombat(game, unit, target, dmg, rec[0x1a5] ? rec[0x1a5] : 0x80);
    env->drainDurability(game, unit, target);
    if (void* skill = env->usedSkill(unit)) env->setParam1(skill, dmg[4] & 1);
    return 1;
}
