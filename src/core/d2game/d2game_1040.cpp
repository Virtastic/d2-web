// 1.13c D2Game+0x1040: set a unit stat, resetting a player's gold (0xE) or stash gold (0xF) to 0 when the value passes its limit.
// Retail takes ESI = unit, EBX = stat, EDI = value. The three D2Common calls are seams so the captured path can be replayed; the
// engine's own copy of this rule is the D2MOO gold helper in PLAYER/Player.cpp.
#include <cstdint>

struct D2G1040Env {
    void    (*setStat)(void* unit, int32_t stat, int32_t value, int32_t layer);
    int32_t (*invGoldLimit)(void* unit);
    int32_t (*stashGoldLimit)(void* unit);
};

// 1.13c D2Game+0x1040
void d2g_1040(void* unit, int32_t stat, int32_t value, const D2G1040Env* env)
{
    if (!unit) return;
    if (value < 0) { env->setStat(unit, stat, 0, 0); return; }
    if (*(const int32_t*)unit == 0) {
        if (stat == 0xE) {
            if (value > env->invGoldLimit(unit)) { env->setStat(unit, 0xE, 0, 0); return; }
        } else if (stat == 0xF) {
            if (value > env->stashGoldLimit(unit)) { env->setStat(unit, 0xF, 0, 0); return; }
        }
    }
    env->setStat(unit, stat, value, 0);
}
