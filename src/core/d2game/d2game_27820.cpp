// D2Game+0x27820: the LeapAttack hit. EAX = unit, stack: game, skill id, level (ret 0xC). A skills.txt row for the id must exist
// and the unit must hold that skill (0xA3F4); the leap target comes from 0x272C0. A damage block (28 dwords, zeroed) takes the
// result flags of the to-hit roll; on a hit: flags |= 8, hit flags |= 0x20, hit class, the elemental-damage percentage (calc 0),
// and, when the row has an element type, the conversion percentage (calc 3; the type is kept only when that percentage is > 0).
// Then the elemental roll (0x9F210), the combat entry (source damage byte at row +0x1A5, 0x80 when 0), the durability drain, the
// server overlay (row +0x4E, 1..overlay count) and the target's state-0x15 list is detached and freed. Returns 0 when there is no
// row, skill or target, else 1. 1.13c delta over the 1.10f port: the skill comes from the id (0xA3F4), not the used skill.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2game_27820_env.h"

static inline uint32_t rd32(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
static inline void wr32(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }

// 1.13c D2Game+0x27820
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2game_27820_env(void* game, void* unit, int32_t id, int32_t level, const D2Game27820Env* env)
{
    if (id < 0) return 0;
    const uint8_t* rec = env->skillRecord(id);
    if (!rec) return 0;
    void* skill = env->skillById(unit, id);
    if (!skill) return 0;
    void* target = env->findTarget(game, skill, unit);
    if (!target) return 0;
    uint8_t dmg[0x70] = {};
    const uint32_t flags = env->resultFlags(game, unit, target, env->toHit(unit, id, level));
    std::memcpy(dmg + 4, &flags, 2);
    if (flags & 1) {
        dmg[4] |= 8;
        wr32(dmg, 0x60, (uint32_t)env->hitClass(unit));
        wr32(dmg, 0, rd32(dmg, 0) | 0x20);
        wr32(dmg, 0xc, (uint32_t)env->formula(unit, (int32_t)rd32(rec, 0x138), id, level));
        if (rec[0x1dc]) {
            const int32_t conv = env->formula(unit, (int32_t)rd32(rec, 0x144), id, level);
            wr32(dmg, 0x68, (uint32_t)conv);
            if (conv > 0) dmg[0x65] = rec[0x1dc];
        }
        env->rollElemental(unit, dmg, id, level);
        env->allocCombat(game, unit, target, dmg, rec[0x1a5] ? rec[0x1a5] : 0x80);
        env->drainDurability(game, unit, target);
        int16_t ov; std::memcpy(&ov, rec + 0x4e, 2);
        if (ov > 0 && ov <= env->overlayCount()) env->setOverlay(target, ov);
        if (void* stats = env->stateList(target, 0x15)) { env->detach(target, stats); env->freeList(stats); }
    }
    return 1;
}
