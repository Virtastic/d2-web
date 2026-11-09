// 1.13c D2Game+0xE36A0: ECX = game, EDX = unit, one unused stack word. Counts a player's quest items on the 0x13 node.
#include "d2game_h16.h"
namespace h16 {
uint32_t e36a0(const Env& e, uint32_t game, uint32_t unit)
{
    uint32_t h = e.o10331(unit);
    if (!h) return 0;
    h = e.o10826(h);
    if (!h || (e.o10459(h) & 0xFF) != 2) return 0;
    uint32_t head = rd32(game, 0x10f4);
    if (!head) return 0;
    uint32_t n = rd32(head, 0);
    while (n && rd32(n, 0) != 0x13) n = rd32(n, 0xf4);
    if (!n) return 0;
    uint32_t d = rd32(n, 0x18);
    uint32_t pl = e.o10920(unit);
    uint32_t item = rd32(pl, 0x10 + rd8(game, 0x6d) * 4);
    if (e.o10174(item, 0x12, 0) == 0 && e.findQuest(unit, game, 0x20316671) == 0 && e.findQuest(unit, game, 0x20326671) == 0)
        wr32(d, 0x3c, rd32(d, 0x3c) + 1);
    if (e.findQuest(unit, game, 0x20786f62) == 0) wr32(d, 0x40, rd32(d, 0x40) + 1);
    return 0;
}
}
