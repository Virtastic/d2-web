// standalone port of D2Game 0x77A50 (marker on d2moo EVENTS_StaminaRegen): one step of a unit's stamina regeneration. EDI = pUnit, no stack args, plain ret.
// Callees are D2Common stat thunks: 0xA42A GetUnitStat(unit, stat, layer), 0xA460 the max-stamina total (unit),
// 0xA472 SetUnitStat(unit, stat, value, layer). The value returned is EAX at retail's `ret` (no declared result):
// the 0x1C stat on the two early exits, the max total when nothing is added, else whatever SetUnitStat leaves.
#include <cstdint>
struct G77A50Env {
    int32_t  (*getStat)(uint32_t unit, uint32_t stat, uint32_t layer);              // 0xA42A
    int32_t  (*maxLife)(uint32_t unit);                                              // 0xA460
    uint32_t (*setStat)(uint32_t unit, uint32_t stat, uint32_t value, uint32_t layer); // 0xA472
};
// standalone port of D2Game 0x77A50
uint32_t g_77a50(uint32_t unit, const G77A50Env& env)
{
    const int32_t cur0 = env.getStat(unit, 10, 0);
    const int32_t pct = env.getStat(unit, 0x1C, 0);
    const uint32_t mode = unit ? *(const uint32_t*)(uintptr_t)(unit + 0x10) : 0;
    int shift = 8;
    switch (mode) {
    case 1: case 5: break;
    case 6: shift = 9; break;
    case 2: shift = 9; if (((uint32_t)cur0 & 0xFFFFFF00u) == 0) return (uint32_t)pct; break;
    default: if (pct < 1000) return (uint32_t)pct; break;
    }
    const int32_t max = env.maxLife(unit);
    if (cur0 >= max) return (uint32_t)max;
    int32_t add = max >> shift;
    if (pct != 0) add += (int32_t)((uint32_t)add * (uint32_t)pct) / 100;
    int32_t cur = (int32_t)((uint32_t)cur0 + (uint32_t)add);
    if (cur > max) cur = max;
    return env.setStat(unit, 10, (uint32_t)cur, 0);
}
