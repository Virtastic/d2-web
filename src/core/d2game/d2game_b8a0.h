// 1.13c D2Game+0xB8A0 (OBJRGN_FreeObjectControl): edi = pGame, plain ret. Written from the 1.13c disassembly as a rule over E (see d2game_c310.h).
// Rule: a null game is a fatal error (not modelled). The object control game+0x10F0, if any: each node of its +0x1110 chain (next at +0xC,
// read before the free) is freed (line 0xC1), the chain head cleared; then for i = 1 .. dt+0xC5C - 1 (the bound reread each pass) the slot
// control+0x4C + 4 * (i - 1) is freed whether null or not (line 0xE2); the eight slots at +0x28 are freed when nonzero (line 0xE8); last
// the control itself, reread from game+0x10F0 (line 0xEC). Every free is Fog's 0xA2EC (ecx = game+0x1C pool, edx = block, stack = the
// objrgn.cpp string, line, 0; ret 0xC), logged with the string as its module tag 0xB20FD174.
#pragma once
#include <cstdint>

namespace b8a0 {
template <class E> uint32_t FreeObjectControl(E& e, uint32_t dt, uint32_t game)
{
    const uint32_t F = 0xB20FD174u;
    if (!game) return 0;
    const uint32_t c = e.rd32(game + 0x10F0);
    if (!c) return 0;
    for (uint32_t n = e.rd32(c + 0x1110); n;) {
        const uint32_t nx = e.rd32(n + 0xC);
        e.h(0xA2EC, 0, e.rd32(game + 0x1C), n, 0, 0, F, 0xC1, 0);
        n = nx;
    }
    e.wr32(c + 0x1110, 0);
    for (int32_t i = 1; i < (int32_t)e.rd32(dt + 0xC5C); i++)
        e.h(0xA2EC, 0, e.rd32(game + 0x1C), e.rd32(c + 0x4C + 4 * (uint32_t)(i - 1)), 0, 0, F, 0xE2, 0);
    for (uint32_t i = 0; i < 8; i++)
        if (e.rd32(c + 0x28 + 4 * i)) e.h(0xA2EC, 0, e.rd32(game + 0x1C), e.rd32(c + 0x28 + 4 * i), 0, 0, F, 0xE8, 0);
    return e.h(0xA2EC, 0, e.rd32(game + 0x1C), e.rd32(game + 0x10F0), 0, 0, F, 0xEC, 0);
}
}
