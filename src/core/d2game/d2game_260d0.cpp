// 1.13c D2Game+0x260D0 -- fire a burst of four offset pairs around a unit (ECX = game, EDX = unit, ret).
// Written from the 1.13c disassembly as a rule over the seam environment (see d2game_c310.h).
// A 23-word request (word 0 = 0x21, word 1 = unit, word 4 = 0xE1, word 5/6 = the unit's x/y, word 12 = 0xA42A(unit, 0xC, 0), word 21 = the callback)
// is handed to 0x6F930(game, request) twice for each of four offset-table steps (the unit's x/y re-read through 0xA454 / 0xA3B8 each step); the block itself
// is not observable through the seam (the register-passed pointer), so only the calls are compared. Returns 0x10 (the loop counter).
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f260d0(GE& e, uint32_t game, uint32_t unit)
{
    e.unitX(unit); e.unitY(unit);
    e.h(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
    uint32_t blk[23] = {};
    for (uint32_t i = 0; i < 16; i += 4) {
        e.unitX(unit); e.unitY(unit);
        for (int j = 0; j < 2; j++) e.h(0x6F930, 0, game, e.stk(blk));
    }
    return 0x10;
}
}
