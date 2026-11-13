// 1.13c D2Game+0xC6390 -- quest callback, ECX = quest record (+4 game, +9 active, +0xC done flag, +0xF0 code pointer), plain ret. An active record (+9 = 1) with +0xC clear sets +0xC = 1, formats "changing state to %d" into a stack buffer (wsprintfA, unobserved) and, when the module debug flag (global 0x111C20) is set, hands the buffer to 0xCBE70 (EDX = buffer, ECX = game, stack 4). It then looks for the kind 0xA node in the game's list holder (+0x10F4; the head pointer is at +0, nodes chain through +0xF4) and calls that node's code pointer (+0xF0, ECX = the node) returning its answer; no holder, no head or no such node leaves EAX with its low byte forced to 1 (the holder pointer's other bytes). The code pointer of the record itself is checked with IsBadCodePtr first (bad is the fatal path, 0x3EE; not vectored). `dbg` is the address of the pinned flag, `node` of the called code pointer's blob.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
static uint32_t slotOf6390(GE& e, uint32_t p)
{
    for (int k = 2; k < e.nb; k++) { const uint32_t b = e.B(k); if (p >= b && p < b + fnBlobLen(e.r, k)) return 0xF00000u + (uint32_t)k * 0x10000u + (p - b); }
    return 0;
}
uint32_t fc6390(GE& e, uint32_t node, uint32_t dbg)
{
    const uint32_t game = e.rd32(node + 4);
    if (!e.rd8(node + 0xc) && e.rd8(node + 9) == 1) {
        e.wr8(node + 0xc, 1);
        char buf[0x80];   // wsprintfA(buf, "changing state to %d\n", 1): the text is only read by 0xCBE70, which the capture records by address
        if (e.rd32(dbg)) e.h(0xCBE70, 0, 0, (uint32_t)(uintptr_t)buf, 0, 0, game, 4);
    }
    const uint32_t holder = e.rd32(game + 0x10f4);
    uint32_t n = holder ? e.rd32(holder) : 0;
    while (n && e.rd32(n) != 0xa) n = e.rd32(n + 0xf4);
    if (!n) return (holder & ~0xffu) | 1;
    return e.h(slotOf6390(e, n + 0xf0), 0, n);
}
}
