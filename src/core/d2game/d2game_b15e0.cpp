// 1.13c D2Game+0xB15E0 -- AI target-scan callback, ECX game, EDX unit, stack word 0 = candidate, word 1 = result struct (ret 8), returns 0. 0xB1400 (ESI = candidate, EDI = unit, stack game) must pass; the 0xB0A20 score (EBX = candidate, stack unit) must be at most 0x30; 0xB08A0 (ECX = candidate) picks the slot pair: a result above 1 uses (+0, +4) and needs score below +4, otherwise (+8, +0xC) needs score below +0xC. 0xAD78 (stack candidate, unit, 4) answering 0 stores the pair.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fb15e0(GE& e, uint32_t game, uint32_t unit, uint32_t cand, uint32_t st)
{
    e.ed(unit);
    const uint32_t ok = e.h(0xB1400, 0, 0, 0, 0, cand, game);
    e.edi(0xB1400, unit);
    if (!ok) return 0;
    const int32_t v = (int32_t)e.h(0xB0A20, 0, 0, 0, cand, 0, unit);
    if (v > 0x30) return 0;
    const int32_t n = (int32_t)e.h(0xB08A0, 0, cand);
    if (n > 1) { if (v >= (int32_t)e.rd32(st + 4)) return 0; }
    else if (v >= (int32_t)e.rd32(st + 0xc)) return 0;
    if (e.h(0xAD78, 0, 0, 0, 0, 0, cand, unit, 4)) return 0;
    if (n > 1) { e.wr32(st, cand); e.wr32(st + 4, (uint32_t)v); }
    else { e.wr32(st + 8, cand); e.wr32(st + 0xc, (uint32_t)v); }
    return 0;
}
}
