// 1.13c D2Game+0xB14B0 -- AI target-scan callback, ECX game, EDX unit, stack target and result struct (ret 8), returns 0. A target other than the unit, of type 1, that 0xDFA90 (EAX target) does not reject and whose 0xB0A20 score (EBX unit, stack target) is below result+0xC, and that 0xE0A60 (EAX target, ECX unit, stack game) does not veto, bumps result+8 and, when the score beats result+4, becomes the best (result+4 score, result+0 target).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fb14b0(GE& e, uint32_t game, uint32_t unit, uint32_t tgt, uint32_t st)
{
    if (unit == tgt || !tgt || e.rd32(tgt) != 1) return 0;
    if (e.h(0xDFA90, tgt)) return 0;
    const int32_t v = (int32_t)e.h(0xB0A20, 0, 0, 0, unit, 0, tgt);
    if (v >= (int32_t)e.rd32(st + 0xc)) return 0;
    if (e.h(0xE0A60, tgt, unit, 0, 0, 0, game)) return 0;
    const int32_t best = (int32_t)e.rd32(st + 4);
    e.wr32(st + 8, e.rd32(st + 8) + 1);
    if (v < best) { e.wr32(st + 4, (uint32_t)v); e.wr32(st, tgt); }
    return 0;
}
}
