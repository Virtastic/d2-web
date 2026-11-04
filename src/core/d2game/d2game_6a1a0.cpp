// 1.13c D2Game+0x6A1A0 -- clear the recorded reference when its owner matches (EAX = game, ECX = message, EDI = owner unit).
// Written from the 1.13c disassembly as a rule over the seam environment. A message whose id (+4) is not 0x151 leaves EAX = game.
// Otherwise 0xB168() returns the address of the reference cell; a non-null owner needs a non-null reference, walks game+0x1120
// (bucket = id & 0x7F, chain through +0xE4, key at +0xC) and must find the owner itself; then the reference is overwritten with -1.
// A null game with an owner reaches the 0x2D3 assertion and exits; it is not modelled.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f6a1a0(GE& e, uint32_t game, uint32_t c, uint32_t owner)
{
    if (!c || e.rd32(c + 4) != 0x151) return game;
    const uint32_t cell = e.h(0xB168);
    const uint32_t p = e.rd32(cell);
    uint32_t eax = cell;
    if (owner) {
        if (!p) return eax;
        const uint32_t id = e.rd32(p);
        uint32_t n = e.rd32(game + 0x1120 + (id & 0x7F) * 4);
        while (n && e.rd32(n + 0xC) != id) n = e.rd32(n + 0xE4);
        eax = n;
        if (n != owner) return eax;
    }
    if (p) e.wr32(p, 0xFFFFFFFFu);
    return eax;
}
}
