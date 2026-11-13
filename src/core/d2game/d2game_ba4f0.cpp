// 1.13c D2Game+0xBA4F0 -- quest callback, ECX = quest record (+4 game, +9 active, +0xC done flag), plain ret, returns 1 (AL; the upper bytes are scratch). An active record (+9 = 1) with +0xC clear sets +0xC = 1, formats "changing state to %d" into a stack buffer (wsprintfA, unobserved) and, when the module debug flag (global 0x111C20) is set, hands it to 0xCBE70 (EDX = buffer, ECX = game, stack words game, 4 -- the caller pushes 4 then ECX).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fba4f0(GE& e, uint32_t node, uint32_t dbg)
{
    if (!e.rd8(node + 0xc) && e.rd8(node + 9) == 1) {
        e.wr8(node + 0xc, 1);
        char buf[0x80];
        if (e.rd32(dbg)) e.h(0xCBE70, 0, e.rd32(node + 4), (uint32_t)(uintptr_t)buf, 0, 0, e.rd32(node + 4), 4);
    }
    return 1;
}
}
