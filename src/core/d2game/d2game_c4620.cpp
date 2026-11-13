// 1.13c D2Game+0xC4620 -- EAX = the caller's, ECX = this (+0x18 word), EDX = event record (+0 game, +0xC unit, +0x14 type word, +0x18 argument word). Type 0xC9 with argument 0xFD: the flag buffer of the unit (0xA4C0(+0xC)) for the game's difficulty (byte +0x6D) gets 0xA57A(buffer, 8, 0), then the worker 0x33920 runs with EAX = game and (8, 0xD). Type 0xF4 with argument 0x7D: the worker 0x33880 runs with EAX = [this+0x18] and ESI = the unit, or -1 when there is none. EAX leaves as the last worker's answer, or the caller's upper half with the type word in AX when nothing ran.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc4620(GE& e, uint32_t eax, uint32_t th, uint32_t ev)
{
    const uint32_t t = e.rd32(ev + 0x14) & 0xffff, w = e.rd32(ev + 0x18) & 0xffff;
    const uint32_t lo = (eax & 0xffff0000u) | t;
    if (t == 0xc9) {
        if (w != 0xfd) return lo;
        const uint32_t game = e.rd32(ev);
        const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ev + 0xc));
        const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
        e.h(0xA57A, 0, 0, 0, 0, 0, buf, 8, 0);
        return e.h(0x33920, game, 0, 0, 0, 0, 8, 0xd);
    }
    if (t != 0xf4 || w != 0x7d) return lo;
    const uint32_t u = e.rd32(ev + 0xc);
    return e.h(0x33880, e.rd32(th + 0x18), 0, 0, 0, u ? e.rd32(u + 0xc) : 0xffffffffu);
}
}
