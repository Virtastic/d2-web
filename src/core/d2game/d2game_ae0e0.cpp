// 1.13c D2Game+0xAE0E0 -- ECX game, EDX mode (6 divides the chance by 3), five stack words (ret 0x14): unit, target (its [0] must be 1), unused, packed (hi, lo), unused. Returns 0 or 1.
// n = 0xA802(unit, hi, lo) must be non-zero. chance = ((0xA42A(unit, 0xC, 0) + 4n - 0xA42A(target, 0xC, 0)) + 6) * 5; mode 6 and chance > 0: chance /= 3; clamp to 1..99.
// A roll 0x1180(ECX unit+0x20, EAX 100) must be below the chance; then m = (chance - roll) / 5 + 1 clamped to 1..20 and 0x50470(game, unit, target, m) runs, result 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fae0e0(GE& e, uint32_t game, uint32_t mode, uint32_t unit, uint32_t tgt, uint32_t packed)
{
    if (!unit || !tgt || e.rd32(tgt) != 1) return 0;
    const int32_t n = (int32_t)e.h(0xA802, 0, 0, 0, 0, 0, unit, packed >> 16, packed);
    if (!n) return 0;
    const int32_t a = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
    const int32_t b = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, tgt, 0xC, 0);
    int32_t c = (int32_t)((uint32_t)(a + (int32_t)((uint32_t)n * 4u) - b + 6) * 5u);
    if (mode == 6 && c > 0) c /= 3;
    if (c < 2) c = 1;
    else if (c >= 0x63) c = 0x63;
    const int32_t roll = (int32_t)e.h(0x1180, 100, unit + 0x20, 0, 0, 0);
    if (roll >= c) return 0;
    int32_t m = (c - roll) / 5 + 1;
    if (m < 2) m = 1;
    else if (m >= 0x14) m = 0x14;
    e.h(0x50470, 0, 0, 0, 0, 0, game, unit, tgt, (uint32_t)m);
    return 1;
}
}
