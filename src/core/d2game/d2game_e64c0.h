// 1.13c D2Game+0xE64C0, transcribed from the 1.13c disassembly: the Act 1 quest (group 0x11) completion step for a player.
// Convention: ECX = game, EDX = player unit, ret 4 (one unused stack word); always returns 0. 0xA478 (D2Common 10331: unit) gives the player's inventory/record pointer (0 ends the
// function); the quest flags are 0xA4C0 (player quest data) + 0x10 + 4 * difficulty (game byte +0x6D), read with 0xA568 (qs, group, bit), set with 0xA57A. When neither bit 0 nor
// bit 1 of group 0x11 is set and the low byte of 0xA56E(0xA556(record)) is 2, bits 0xD, 0 and 1 are set and 0x34C00 (EAX = unit, stack game) runs.
#pragma once
#include <cstdint>

namespace re64c0 {
template <class E> uint32_t Do(E& e, uint32_t game, uint32_t unit, uint32_t a1)
{
    (void)a1;
    const uint32_t rec = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!rec) return 0;
    const uint32_t pq = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t qf = e.rd32(pq + e.rd8(game + 0x6D) * 4 + 0x10);
    if (e.h(0xA568, 0, 0, 0, 0, 0, qf, 0x11, 0)) return 0;
    if (e.h(0xA568, 0, 0, 0, 0, 0, qf, 0x11, 1)) return 0;
    const uint32_t k = e.h(0xA556, 0, 0, 0, 0, 0, rec);
    if ((e.h(0xA56E, 0, 0, 0, 0, 0, k) & 0xFF) != 2) return 0;
    e.h(0xA57A, 0, 0, 0, 0, 0, qf, 0x11, 0xD);
    e.h(0xA57A, 0, 0, 0, 0, 0, qf, 0x11, 0);
    e.h(0xA57A, 0, 0, 0, 0, 0, qf, 0x11, 1);
    e.h(0x34C00, unit, 0, 0, 0, 0, game);
    return 0;
}
}
