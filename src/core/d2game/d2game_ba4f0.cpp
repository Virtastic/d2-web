// 1.13c D2Game+0xBA4F0 -- quest callback, ECX = quest record (+4 game, +9 active, +0xC done flag), plain ret, returns 1 (AL; the upper bytes are scratch). An active record (+9 = 1) with +0xC clear sets +0xC = 1, formats "changing state to %d" into a stack buffer (wsprintfA, unobserved) and, when the module debug flag (global 0x111C20) is set, hands it to 0xCBE70 (EDX = buffer, ECX = game, stack 4).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fba4f0(GE& e, uint32_t node, uint32_t dbg)
{
    if (!e.rd8(node + 0xc) && e.rd8(node + 9) == 1) {
        e.wr8(node + 0xc, 1);
        char buf[0x80];
        if (e.rd32(dbg)) { const uint32_t g = e.rd32(node + 4); e.h(0xCBE70, 0, g, (uint32_t)(uintptr_t)buf, 0, 0, g, 4); }   // push 4; push ecx; ECX game, EDX buf
    }
    return 1;
}
}
