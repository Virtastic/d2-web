// 1.13c D2Game+0x96AF0 -- visit every live pet of a player (stack = player, ret 8; EDI = game word, EBX = callback).
// Written from the 1.13c disassembly as a rule over E. rec = 0xA4C0(player); rec+0x44 = the pet table (zero: return), whose first word is the array of
// 0xC-byte slots; slots 1 .. count-1 (count = sgptDataTables +0xBF0, re-read each pass) each hold a list head, nodes linked through +0x14. A node with bit 0
// of its flags clear looks its id (+4) up in the game's type-1 hash (game+0x1320 + (id&0x7F)*4, chain +0xE4, id at +0xC); a hit calls
// cb(ECX = game, EDX = player, stack = unit, player). A null callback or null game with a node to visit is fatal in retail (not modelled).
#ifndef D2GAME_RULE_96AF0
#define D2GAME_RULE_96AF0
#include <cstdint>

namespace g96af0 {
template <class E> void Each(E& e, uint32_t game, uint32_t cb, uint32_t player, uint32_t g, uint32_t extra = 0)
{
    const uint32_t rec = e.h(0xA4C0, 0, 0, 0, 0, 0, player);
    (void)cb;
    const uint32_t p44 = e.rd32(rec + 0x44);
    if (!p44) return;
    for (int32_t i = 1; i < (int32_t)e.rd32(e.rd32(g) + 0xBF0); ++i) {
        const uint32_t slot = e.rd32(p44) + 0xCu * (uint32_t)i;
        if (!slot) continue;
        for (uint32_t nd = e.rd32(slot); nd; nd = e.rd32(nd + 0x14)) {
            if (e.rd32(nd) & 1u) continue;
            const uint32_t key = e.rd32(nd + 4);
            uint32_t u = e.rd32(game + 0x1320u + (key & 0x7Fu) * 4u);
            while (u && e.rd32(u + 0xC) != key) u = e.rd32(u + 0xE4);
            if (u) e.h(0x97A20, 0, game, player, 0, 0, u, extra);
        }
    }
}
}
#endif
