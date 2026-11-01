// 1.13c D2Game+0x26320 -- throttled burst of four offset pairs around a unit (ECX = game, EDX = unit, [esp+4] = a, [esp+8] = b, ret 8).
// Written from the 1.13c disassembly as a rule over the seam environment (see d2game_c310.h).
// b == 0 -> 0. Frame = game +0xA8; a type-1 unit's pData +0x18 holds the last frame. Within 10 frames: clear pData +0x17 bit 0 (type 1 only), return the
// distance. Otherwise store the frame, set +0x17 bit 0, step = max(1, 0xA42A(unit, 0xC, 0) / 2 rounded toward zero), build the request (word 0 = 0x21,
// word 4 = 0xC3, word 12 = step) and hand it to 0x6F930(game, request) twice for each of four offset-table steps (x/y read once through 0xA454 / 0xA3B8 twice before the loop).
// The request block is not observable through the seam (register-passed pointer). Returns the last 0x6F930 result.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f26320(GE& e, uint32_t game, uint32_t unit, uint32_t a, uint32_t b)
{
    (void)a;
    if (!b) return 0;
    const bool u1 = unit && e.rd32(unit) == 1;
    const uint32_t pd = u1 ? e.rd32(unit + 0x14) : 0, fr = e.rd32(game + 0xA8);
    int32_t d = (int32_t)(fr - (pd ? e.rd32(pd + 0x18) : 0)); if (d < 0) d = -d;
    if (d < 10) {
        if (u1) { const uint32_t p = e.rd32(unit + 0x14); if (p) *(uint8_t*)(uintptr_t)(p + 0x17) &= 0xFE; }
        return (uint32_t)d;
    }
    if (u1) {
        uint32_t p = e.rd32(unit + 0x14);
        if (p) e.wr32(p + 0x18, fr);
        if (e.rd32(unit) == 1 && (p = e.rd32(unit + 0x14)) != 0) *(uint8_t*)(uintptr_t)(p + 0x17) |= 1;
    }
    e.h(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
    e.unitX(unit); e.unitY(unit);   // the request words 5 and 6
    e.unitX(unit); e.unitY(unit);   // re-read for the offset sums
    uint32_t blk[23] = {}, r = 0;
    for (uint32_t i = 0; i < 16; i += 4)
        for (int j = 0; j < 2; j++) r = e.h(0x6F930, 0, game, e.stk(blk));
    return r;
}
}
