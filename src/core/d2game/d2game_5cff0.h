// 1.13c D2Game+0x5CFF0: fastcall(ECX = game, EDX = unit, one stack word, ret 4). Finds the unit's player
// record in the game's 128-bucket hash at +0x1520 and, when the node's unit class is 0x78, runs the 0x386F0 rule (EAX = unit, EBX = game).
// Every callee is a seam on E: E::h(rva, eax, ecx, edx, ebx, esi, stack words...) and E::rd32.
#pragma once
#include "d2game_fatal.h"

namespace d2g5cff0 {
template <class E> uint32_t Run(E& e, uint32_t game, uint32_t unit, uint32_t mod)
{
    uint32_t line;
    if (unit == 0) line = 0x24C;
    else {
        const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);   // Fog #10920
        if (pd == 0) line = 0x24F;
        else if (game == 0) line = 0x2D3;
        else {
            const uint32_t key = e.rd32(pd + 0x48);
            uint32_t n = e.rd32(game + 0x1520 + (key & 0x7F) * 4);
            while (n != 0 && e.rd32(n + 0xC) != key) n = e.rd32(n + 0xE4);
            if (n != 0) {
                const uint32_t u = e.h(0xA478, 0, 0, 0, 0, 0, n);   // #10331
                if (e.h(0xA556, 0, 0, 0, 0, 0, u) == 0x78) e.h(0x386F0, unit, 0, 0, game);   // #10826
            }
            return 0;
        }
    }
    d2gfatal::Fatal(e, line, mod);
    return 0;
}
}
