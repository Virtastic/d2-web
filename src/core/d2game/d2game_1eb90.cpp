// Decompile-derived, 1.13c-only. Seams: D2Common #10561/#10826/#10459 and the inactive-list lookup (D2Game+0x1E960).
#include <cstdint>

struct D2Game1eb90Env {
    void (*getCoords)(uint32_t unit, uint32_t out[2]);                                  // D2Common #10561 (unit, &xy)
    uint32_t (*getRoom)(uint32_t unit);                                                 // D2Common #10826
    uint32_t (*getAct)(uint32_t room);                                                  // D2Common #10459
    void (*lookup)(uint32_t arg0, uint32_t act, uint32_t y, uint32_t arg1, uint32_t x, uint32_t yReg);   // 0x1E960: 4 stack words, EBX = x, EDX = y
};

// 1.13c D2Game+0x1EB90
// ESI = unit, stack arg0/arg1 (ret 8). Looks the unit's position and act up and forwards to 0x1E960.
extern "C" void d2game_item_slot_lookup_1eb90(uint32_t unit, uint32_t arg0, uint32_t arg1, const D2Game1eb90Env* env)
{
    uint32_t xy[2];
    env->getCoords(unit, xy);
    const uint32_t act = env->getAct(env->getRoom(unit)) & 0xff;
    env->lookup(arg0, act, xy[1], arg1, xy[0], xy[1]);
}
