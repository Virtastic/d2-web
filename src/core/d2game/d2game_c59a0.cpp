// 1.13c D2Game+0xC59A0 -- quest-event callback, ECX = game, EDX = unit, one ignored stack word (ret 4), returns 0. The unit's quest-flag buffer (0xA4C0, byte game+0x6D) is taken first; 0xA478(unit) = 0 ends it. The game's list holder (+0x10F4) must hold a kind 0xD node (else the fatal path). When 0xA556 of that answer is 0x49 and state 0xE flags 0xD, 3 and 4 are all clear, 0xC4C30 (EAX = buffer, ECX = unit, game on the stack) runs.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc59a0(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t ent = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    const uint32_t a = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!a) return 0;
    uint32_t n = 0;
    const uint32_t holder = e.rd32(game + 0x10f4);
    if (holder)
        for (n = e.rd32(holder); n && e.rd32(n) != 0xd; ) n = e.rd32(n + 0xf4);
    if (!n) {   // fatal path, not vectored
        const uint32_t msg = e.h(0xA2DA, 0, 0, 0, 0, 0, 0x191);
        e.h(0xA2C2, 0, 0, 0, 0, 0, 0x6fd19ff9, msg);
        e.h(0x2A3C, 0, 0, 0, 0, 0, 0xffffffffu);
        return 0;
    }
    const bool is49 = e.h(0xA556, 0, 0, 0, 0, 0, a) == 0x49;
    static const uint32_t fl[3] = { 0xd, 3, 4 };
    for (int i = 0; i < 3; ++i)
        if (e.h(0xA568, 0, 0, 0, 0, 0, ent, 0xe, fl[i])) return 0;
    if (is49) e.h(0xC4C30, ent, unit, 0, 0, 0, game);
    return 0;
}
}
