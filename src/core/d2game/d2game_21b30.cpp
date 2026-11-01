// Decompile-derived, 1.13c-only. Seams: D2Common stat get (#10973, D2Game+0xA42A), owner lookup (#10195, +0xA7B4), stat set (#10188, +0xA6E2).
#include <cstdint>
#include <cstring>

struct D2Game21b30Env {
    int32_t (*getStat)(uint32_t unit, uint32_t id, uint32_t sub);
    uint32_t (*getOwner)(uint32_t unit, uint32_t flag);
    void (*setStat)(uint32_t unit, uint32_t id, int32_t v, uint32_t sub);
};

// 1.13c D2Game+0x21B30
// EDX = unit, two unused stack words (ret 8). stat 0x16 > 0 and the unit type (unit+4) != 0x10F: stats 0x15 and 0x16 become
// (stat + stat 0x17/0x18 - 1) clamped at 0, written to the unit's owner (#10195(unit, 1)).
extern "C" void d2game_fold_stat_box_21b30(uint32_t unit, const D2Game21b30Env* env)
{
    int32_t x = env->getStat(unit, 0x15, 0);
    int32_t y = env->getStat(unit, 0x16, 0);
    if (y <= 0) return;
    if (unit && *(const uint32_t*)(uintptr_t)(unit + 4) == 0x10f) return;
    x = (int32_t)((uint32_t)x + (uint32_t)env->getStat(unit, 0x17, 0) - 1);
    y = (int32_t)((uint32_t)y + (uint32_t)env->getStat(unit, 0x18, 0) - 1);
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    const uint32_t owner = env->getOwner(unit, 1);
    env->setStat(owner, 0x15, x, 0);
    env->setStat(owner, 0x16, y, 0);
}
