// 1.13c D2Game+0xAD7B0 -- ECX game, five stack words (ret 0x14): unit, target, (unused), packed (hi word, lo word). With both non-null and target+0xC4 bit 2 set and 0xA802(unit, hi, lo) positive (=n), builds a zeroed 0x70-byte stack record (+4 = 0x4021, +0x10 = n << 8, +0x60 = 0x2d) and hands it to 0xDE0C0 (EAX target, stack game, unit, 1, record) and 0xDCF20 (EAX target, stack game, unit, record); returns 1, else 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fad7b0(GE& e, uint32_t game, uint32_t unit, uint32_t tgt, uint32_t packed)
{
    if (!unit || !tgt || !(e.rd32(tgt + 0xc4) >> 2 & 1)) return 0;
    const int32_t n = (int32_t)e.h(0xA802, 0, 0, 0, 0, 0, unit, packed >> 16, packed);
    if (n <= 0) return 0;
    uint32_t rec[28] = {};
    rec[1] |= 0x4021;
    rec[4] = (uint32_t)n << 8;
    rec[24] = 0x2d;
    const uint32_t p = e.stk(rec);
    e.hs(0xDE0C0, tgt, 0, 0, 0, 0, {game, unit, 1, p}, 3, 28);
    e.hs(0xDCF20, tgt, 0, 0, 0, 0, {game, unit, p}, 2, 28);
    return 1;
}
}
