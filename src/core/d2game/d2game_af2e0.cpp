// 1.13c D2Game+0xAF2E0 -- ECX game, five stack words (ret 0x14): unit, target, (unused), packed (hi word, lo word), returns 0 or 1. Needs both non-null, target type 1 and no 0xC bits in the byte at (target+0x14)+0x16; 0xA802(unit, hi, lo) = n must be positive and 0x1450 (stack 0x80, ESI = unit+0x20) must answer below n; then 0xB2120 (EAX unit, ECX game, ESI target, stack 0x14) and 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t faf2e0(GE& e, uint32_t game, uint32_t unit, uint32_t tgt, uint32_t packed)
{
    if (!unit || !tgt || e.rd32(tgt) != 1) return 0;
    const uint32_t p = e.rd32(tgt + 0x14);
    if (p && (e.rd8(p + 0x16) & 0xc)) return 0;
    const int32_t n = (int32_t)e.h(0xA802, 0, 0, 0, 0, 0, unit, packed >> 16, packed);
    if (n <= 0) return 0;
    if ((int32_t)e.h(0x1450, 0, 0, 0, 0, unit + 0x20, 0x80) >= n) return 0;
    e.h(0xB2120, unit, game, 0, 0, tgt, 0x14);
    return 1;
}
}
