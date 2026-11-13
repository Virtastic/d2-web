// 1.13c D2Game+0xAF210 -- ECX game, five stack words (ret 0x14): unit, target, (unused), packed (hi word, lo word), returns 0 or 1. Same gates as 0xAF2E0
// (both non-null, target type 1, no 0xC bits in the byte at (target+0x14)+0x16); 0xA802(unit, hi, lo) = n must be positive; then the seed roll 0x11D0
// (ECX unit+0x20, ESI 100) must answer below n. The unit is then walked up through 0x21260 (ECX unit; 0 ends) to the first type-0 unit, and
// 0x37190 (ECX game, ESI target, EBP packed, EDI = that unit's id, ret 0x1C: game, 0xD, lo, id, 0xAE200, 0, id) is called with result 1.
// Retail's walk tests the unit for null each pass but still calls 0x21260 with a null ECX (a no-op returning 0); we mirror the call.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t faf210(GE& e, uint32_t game, uint32_t unit, uint32_t tgt, uint32_t packed)
{
    if (!unit || !tgt || e.rd32(tgt) != 1) return 0;
    const uint32_t p = e.rd32(tgt + 0x14);
    if (p && (e.rd8(p + 0x16) & 0xc)) return 0;
    const int32_t n = (int32_t)e.h(0xA802, 0, 0, 0, 0, 0, unit, packed >> 16, packed);
    if (n <= 0) return 0;
    if ((int32_t)e.h(0x11D0, 0, unit + 0x20, 0, 0, 0x64) < n) {
        uint32_t u = unit;
        for (;;) {
            if (u && e.rd32(u) == 0) {
                const uint32_t id = e.rd32(u + 0xc);
                const uint32_t w[7] = { game, 0xd, packed & 0xffff, id, 0xB2000000u | 0xAE200u, 0, id };
                e.h(0x37190, 0, game, 0, 0, tgt, w[0], w[1], w[2], w[3]);
                e.x(0x37190, w + 4, 3);
                e.y(0x37190, packed, id);
                return 1;
            }
            u = e.h(0x21260, 0, u);
            if (!u) break;
        }
    }
    return 0;
}
}
