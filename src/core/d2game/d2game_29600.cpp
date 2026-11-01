// 1.13c D2Game+0x29600 -- skill launch at the unit's target (ECX = game, EDX = unit, [esp+4] = a, [esp+8] = b, ret 8).
// Written from the 1.13c disassembly as a rule over the seam environment (see d2game_c310.h).
// Refresh the target (0xE07A0), resolve unit +0x2C (0xA598); none or itself -> 0. Unit +0x38 bit 8 clear: 0xDED30(game, unit, target),
// 0xDA6D0(game, target), 0x79100(game), 0x78A90(game), 0x28E70(game, a, b), return 0x27A10(a; game, unit, target, b). Set: unit +0xC4 |= 0x40, the same
// first four calls, then queue a 6-word request through 0xA0C70 (frames 0xA77E + 4, target unit id twice), word 0 else word 2, and 0x27A10 with it.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f29600(GE& e, uint32_t game, uint32_t unit, uint32_t a, uint32_t b)
{
    e.h(0xE07A0, unit, game);
    const uint32_t tgt = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (tgt == unit || !tgt) return 0;
    const bool set = (e.rd32(unit + 0x38) >> 8 & 1u) != 0;
    if (set) e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 0x40);
    e.h(0xDED30, 0, 0, 0, 0, 0, game, unit, tgt);
    if (!set) e.h(0xDA6D0, 0, 0, 0, 0, 0, game, tgt);
    e.h(0x79100, unit, 0, 0, 0, 0, game);
    e.h(0x78A90, 0, 0, 0, 0, 0, game);
    e.h(0x28E70, 0, 0, 0, 0, 0, game, a, b);
    uint32_t res = tgt;
    if (set) {
        const uint32_t n = e.h(0xA77E, 0, 0, 0, 0, 0, unit);
        const uint32_t id = e.rd32(tgt + 0xC);
        uint32_t blk[6] = { 0, 0xFFFFFFFFu, 0, id, id, 0 };
        e.hs(0xA0C70, game, 0, 0, 0, 0, { unit, 0, 0, n + 4, 0x2A783, 0, e.stk(blk), 0 }, 6, 6);
        res = blk[0];
        if (!res) res = blk[2];
    }
    return e.h(0x27A10, a, 0, 0, 0, 0, game, unit, res, b);
}
}
