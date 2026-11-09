// 1.13c D2Game+0xA5C80 (ECX = game, EDX = unit, stack ctx, ret 4)
#include "d2game_a5c80.h"
#include <cstring>

static uint32_t rd(const void* p, uint32_t off) { uint32_t v; memcpy(&v, (const uint8_t*)p + off, 4); return v; }
static const uint8_t* ptr(const void* p, uint32_t off) { return (const uint8_t*)(uintptr_t)rd(p, off); }

void d2game_a5c80(const D2GameA5C80Env& env, void* game, uint8_t* unit, const uint8_t* ctx)
{
    const int32_t limit = (int32_t)rd(ctx, 0x14);
    const uint8_t* inner = nullptr;
    bool queued = false;
    if (unit && rd(unit, 0) == 1 && (inner = ptr(unit, 0x14)) != nullptr) {
        const uint8_t* a = ptr(inner, 0x28);
        const uint8_t* b = a ? ptr(a, 0x20) : nullptr;
        queued = b && rd(b, 8) == 8;
    }
    if (!queued) {
        uint32_t ref = 0;
        env.lookupRef(unit, &ref);
        const bool mode3 = unit && rd(unit, 0) == 1 && inner && (rd(inner, 0x54) == 3 || rd(inner, 0x54) == 0x13);
        if (!mode3 && ref == 0 && limit >= 0x14) { env.defaultAction(game, 10); return; }
        uint32_t blk[7];   // only word 2 is set; words 3..6 are stack residue the callee copies
        memset(blk, 0, sizeof blk);
        blk[2] = 8;
        env.allocCommand(game, unit, blk);
    }
    env.freeMinions(unit);
    env.executeAi(game, unit, rd(ctx, 0), 0);
    env.defaultAction(game, 10);
}
