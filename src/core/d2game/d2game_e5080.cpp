// 1.13c D2Game+0xE5080 -- ECX game, EDX = value handed on in EAX, one unused stack word (ret 4). When 0xE660 (EAX = EDX, stack 0x20626262) answers nonzero, the kind 0xF node of the game's list (holder at +0x10F4, head at +0, nodes chain through +0xF4) has the byte at (node+0x18)+4 set to 1. No holder, head or node is the null-node fault path (not vectored).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
void fe5080(GE& e, uint32_t game, uint32_t edx)
{
    if (!e.h(0xE660, edx, 0, 0, 0, 0, 0x20626262)) return;
    const uint32_t holder = e.rd32(game + 0x10f4);
    uint32_t n = holder ? e.rd32(holder) : 0;
    while (n && e.rd32(n) != 0xf) n = e.rd32(n + 0xf4);
    e.wr8(e.rd32(n + 0x18) + 4, 1);
}
}
