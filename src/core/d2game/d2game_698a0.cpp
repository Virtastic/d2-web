// 1.13c D2Game+0x698A0 -- bring stat 6 up to its maximum (ECX = effect context; the unit is context+8).
// Written from the 1.13c disassembly as a rule over the seam environment: every callee is one e.h call.
// cur = 0xA42A(unit, 6, 0); max = 0xA3A0(unit); when they differ, 0xA4D2(unit, 6, max - cur, 0) adds the difference.
// The result is the last callee's value, or 0 when nothing was added.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f698a0(GE& e, uint32_t c)
{
    const uint32_t u = e.rd32(c + 8);
    const uint32_t cur = e.h(0xA42A, 0, 0, 0, 0, 0, u, 6, 0);
    const uint32_t d = e.h(0xA3A0, 0, 0, 0, 0, 0, u) - cur;
    if (!d) return 0;
    return e.h(0xA4D2, 0, 0, 0, 0, 0, u, 6, d, 0);
}
}
