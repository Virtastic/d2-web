// 1.13c D2Game+0xF02F0: ECX = player, EAX = other, stack game (ret 4). pd = #10920(player) (0xA4C0). Nothing more when player == other,
// #10494(other, 7) != 0 (0x2A68E), #10494(player, 7) != 0, or 0xF0060 (ESI = other, EDI = player) finds a node. Otherwise a 0x14-byte node comes
// from 0xA2E6 (ECX = *(game+0x1C), EDX = 0x14, stack file "..\Source\D2Game\UNIT\PlayerList.cpp", 0x6D, 0), is zeroed and gets [0] = other's id
// (-1 for null), [8] = 0, [0xC] = (0xDFA60(EAX = other) as a 16-bit value != 0xFFFF), [0x10] = old head; pd+0x38 = node.
#pragma once
#include <cstdint>
namespace gf02f0 {
template <class E> void Run(E& e, uint32_t eax, uint32_t ecx, uint32_t game)
{
    // 1.13c D2Game+0xF02F0
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, ecx);
    if (ecx == eax) return;
    if (e.h(0x2A68E, 0, 0, 0, 0, 0, eax, 7)) return;
    if (e.h(0x2A68E, 0, 0, 0, 0, 0, ecx, 7)) return;
    const uint32_t f = e.h(0xF0060, 0, 0, 0, 0, eax); e.j(0xF0060, ecx);
    if (f) return;
    const uint32_t nn = e.h(0xA2E6, 0, e.rd32(game + 0x1C), 0x14, 0, 0, 0xB20FB9A4u, 0x6D, 0);
    for (uint32_t i = 0; i < 5; i++) e.wr32(nn + 4 * i, 0);
    e.wr32(nn, eax ? e.rd32(eax + 0xC) : 0xFFFFFFFFu);
    e.wr32(nn + 8, 0);
    const uint32_t s = e.h(0xDFA60, eax);
    e.wr32(nn + 0xC, (uint16_t)s != 0xFFFFu);
    e.wr32(nn + 0x10, e.rd32(pd + 0x38));
    e.wr32(pd + 0x38, nn);
}
}
