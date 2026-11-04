// 1.13c D2Game+0x6B8A0 -- start a path move for the context's unit (ECX = context: +0 game, +8 unit).
// Written from the 1.13c disassembly as a rule over the seam environment. The unit's two coordinates come through the type switch
// of GE: the +0x10 / 0xA3B8 value is read first. 0xB16E(unit, +0xC value, +0x10 value, 0, callback 0x6A440) starts the move;
// a non-zero answer tail-jumps to 0x24B70 (EAX = the answer, ECX = game), whose result is returned; otherwise 0.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f6b8a0(GE& e, uint32_t c)
{
    const uint32_t u = e.rd32(c + 8);
    const uint32_t first = e.unitY(u);
    const uint32_t second = e.unitX(u);
    const uint32_t r = e.h(0xB16E, 0, 0, 0, 0, 0, u, second, first, 0);
    if (!r) return 0;
    return e.h(0x24B70, r, e.rd32(c));
}
}
