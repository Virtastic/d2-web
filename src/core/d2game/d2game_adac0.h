// Player item-use gate: ECX = game, EDX = player, stack = 5 words of which word 1 is the item (ret 0x14). With byte player+0x6C set and
// player[0x68/4] == 0, a nonzero 0xE03A0(EAX = 0, ECX = game, EDX = player[0x64/4]) refuses (0). A nonzero #A8A4(item, 0x10, 0x94, file)
// refuses (0). Then a nonzero 0xAC9E0(EAX = player) gets 0xD6BC0(EAX = player; game) and 1; else 0.
#pragma once
#include <cstdint>

namespace gadac0 {
static constexpr uint32_t kFile = 0xB20FB660u;   // the source-file string handed to the ordinal (module address)
template <class E> uint32_t Use(E& e, uint32_t game, uint32_t player, uint32_t item)
{
    // 1.13c D2Game+0xADAC0
    if (e.rd8(player + 0x6C) && !e.rd32(player + 0x68) && e.h(0xE03A0, 0, game, e.rd32(player + 0x64))) return 0;
    if (e.h(0xA8A4, 0, 0, 0, 0, 0, item, 0x10, 0x94, kFile)) return 0;
    if (!e.h(0xAC9E0, player)) return 0;
    e.h(0xD6BC0, player, 0, 0, 0, 0, game);
    return 1;
}
}
