// 1.13c D2Game+0x15D10 (AI: make sure a unit has a home command, param 10, and fill it with the unit's position): EAX = unit,
// stack = pGame (ret 4). Written from the 1.13c disassembly as a rule over E (see d2game_d040.h).
// Rule: the unit's AI control ctl = unit(type 1) pData(+0x14) +0x28 +0x20 is the head of a circular command list (node +0 = next,
// +8 = param 0). A node with param 0 == 10 is searched from the head's first node until it is found or the walk returns to the head
// (the head itself is then tested). Not found, or no control: 0x21290 (EAX = game, ECX = unit, stack = a command block whose words
// 2..6 are {10, 0, 0, 0, 0}) copies one in. The list is searched again; a node with both coordinates (+0xC, +0x10) zero gets the
// unit's X (0x1100) and Y (0x1130) and the unit idles 20 frames (0xF22C0, EAX = game, ESI = unit, stack 0x14): result 1. Else 0.
#pragma once
#include <cstdint>

namespace g15d10 {
template <class E> uint32_t EnsureHome(E& e, uint32_t unit, uint32_t game)
{
    // 1.13c D2Game+0x15D10
    auto head = [&]() -> uint32_t {
        if (!unit || e.rd32(unit) != 1) return 0;
        const uint32_t pd = e.rd32(unit + 0x14);
        if (!pd) return 0;
        const uint32_t ai = e.rd32(pd + 0x28);
        if (!ai) return 0;
        return e.rd32(ai + 0x20);
    };
    auto find = [&](uint32_t h) -> uint32_t {
        uint32_t n = e.rd32(h);
        while (e.rd32(n + 8) != 10 && n != h) n = e.rd32(n);
        return e.rd32(n + 8) == 10 ? n : 0;
    };
    uint32_t h = head();
    if (!h || !find(h)) {
        uint32_t blk[7] = {};
        blk[2] = 10;
        e.h(0x21290, game, unit, 0, 0, 0, e.stk(blk));
        e.blk(0x21290, blk, 7, 0x7C);
    }
    h = head();
    if (!h) return 0;
    const uint32_t c = find(h);
    if (!c || e.rd32(c + 0xC) || e.rd32(c + 0x10)) return 0;
    e.wr32(c + 0xC, e.h(0x1100, 0, unit));
    e.wr32(c + 0x10, e.h(0x1130, 0, unit));
    e.h(0xF22C0, game, 0, 0, 0, unit, 0x14);
    return 1;
}
}
