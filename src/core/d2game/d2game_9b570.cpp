// Unit-list ownership test: ECX = unit, EBX = game (plain ret). A unit with flag bit 22 or 23 of +0xC8 counts at once (1).
// Otherwise its list at +0x60 is walked (first node, node id, next node: three stdcall queries); each id is looked up in the
// game's 128-bucket hash at +0x1720 (bucket id & 0x7F, chain +0xE4, unit id at +0xC). The next node is asked before the hit
// is acted on; a hit returns 1, an exhausted list 0. A rule over E (E::h = the callee call, E::rd32 = memory).
#ifndef D2GAME_RULE_9B570
#define D2GAME_RULE_9B570
#include <cstdint>

namespace g9b570 {
template <class E> uint32_t AnyListed(E& e, uint32_t unit, uint32_t game)
{
    const uint32_t f = e.rd32(unit + 0xC8);
    if ((f >> 22 & 1) || (f >> 23 & 1)) return 1;
    uint32_t n = e.h(0xAF40, 0, 0, 0, 0, 0, e.rd32(unit + 0x60));
    while (n) {
        const uint32_t id = e.h(0xAF3A, 0, 0, 0, 0, 0, n);
        uint32_t c = e.rd32(game + 0x1720 + (id & 0x7F) * 4);
        while (c && e.rd32(c + 0xC) != id) c = e.rd32(c + 0xE4);
        n = e.h(0xAF46, 0, 0, 0, 0, 0, n);
        if (c) return 1;
    }
    return 0;
}
}
#endif
