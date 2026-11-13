// 1.13c D2Game+0xC5910 -- quest-event callback, ECX = game, EDX = unit, one ignored stack word (ret 4), leftover EAX mirrored. Needs the kind 0xD node in the game's list holder (+0x10F4; none returns 0). Then, with the unit's quest-flag buffer (0xA4C0, byte game+0x6D): state 0xE flags 0, 3 and 4 must each differ from 1 (a 1 is returned), 0xA478(unit) must be nonzero, and 0xA56E(0xA556(it)) must answer 1 in AL; then 0xC4C30 (EAX = buffer, ECX = unit, game on the stack) runs and its answer is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc5910(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t holder = e.rd32(game + 0x10f4);
    if (!holder) return 0;
    uint32_t n = e.rd32(holder);
    while (n && e.rd32(n) != 0xd) n = e.rd32(n + 0xf4);
    if (!n) return 0;
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t ent = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    static const uint32_t fl[3] = { 0, 3, 4 };
    for (int i = 0; i < 3; ++i) {
        const uint32_t r = e.h(0xA568, 0, 0, 0, 0, 0, ent, 0xe, fl[i]);
        if (r == 1) return r;
    }
    const uint32_t a = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!a) return a;
    const uint32_t t = e.h(0xA556, 0, 0, 0, 0, 0, a);
    const uint32_t v = e.h(0xA56E, 0, 0, 0, 0, 0, t);
    if ((v & 0xff) != 1) return v;
    return e.h(0xC4C30, ent, unit, 0, 0, 0, game);
}
}
