// 1.13c D2Game+0xC3E10 -- quest-event callback, EDX = event record (+0 game, +8 trigger {+4 id}, +0xC unit), plain ret (the leftover EAX is mirrored: the last callee answer). The item record of the trigger id (0xA53E; -1 without a trigger) has a type tag at +0x80: "viq " clears state 0xA bit 4, "xob" ("box ") bit 6, "msf " ("fsm ") bit 5 in the unit's quest-flag buffer (byte game+0x6D selects the buffer), each only when 0xA568 says it is set.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc3e10(GE& e, uint32_t ctx)
{
    const uint32_t p8 = e.rd32(ctx + 8);
    const uint32_t rec = e.h(0xA53E, 0, 0, 0, 0, 0, p8 ? e.rd32(p8 + 4) : ~0u);
    const uint32_t unit = e.rd32(ctx + 0xc), game = e.rd32(ctx);
    uint32_t eax = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t ent = e.rd32(eax + 0x10 + 4 * e.rd8(game + 0x6d));
    static const uint32_t tag[3] = { 0x20706976u, 0x20786f62u, 0x2066736du }, slot[3] = { 4, 6, 5 };
    for (int i = 0; i < 3; ++i) {
        if (e.rd32(rec + 0x80) != tag[i]) continue;
        eax = e.h(0xA568, 0, 0, 0, 0, 0, ent, 0xa, slot[i]);
        if (eax) eax = e.h(0xA586, 0, 0, 0, 0, 0, ent, 0xa, slot[i]);
    }
    return eax;
}
}
