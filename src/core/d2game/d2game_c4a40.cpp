// 1.13c D2Game+0xC4A40 -- EAX = the caller's, ECX = this, EDX = event record (+0 game, +0xC unit, +0x14 type word, +0x18 argument word). Only type 0x179 acts: the flag buffer of the unit (0xA4C0(+0xC)) for the game's difficulty (byte +0x6D) gets 0xA57A(buffer, 0x1E, 0xD) for argument 0x3B or 0x3C (and this+0xA8 = callback 0xC27C0), else 0xA57A(buffer, 0x1E, 0). EAX leaves as the setter's answer, or untouched for any other type.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc4a40(GE& e, uint32_t eax, uint32_t th, uint32_t ev)
{
    if ((e.rd32(ev + 0x14) & 0xffff) != 0x179) return eax;
    const uint32_t w = e.rd32(ev + 0x18) & 0xffff, game = e.rd32(ev);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ev + 0xc));
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (w == 0x3b || w == 0x3c) {
        const uint32_t v = e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0x1e, 0xd);
        e.wr32(th + 0xa8, 0xB2000000u | 0xC27C0);
        return v;
    }
    return e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0x1e, 0);
}
}
