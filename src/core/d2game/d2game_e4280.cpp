// 1.13c D2Game+0xE4280: ECX = game, EDX = unit, one unused stack word; returns 0.
#include "d2game_h16.h"
namespace h16 {
uint32_t e4280(const Env& e, uint32_t game, uint32_t unit)
{
    uint32_t sk = rd32(e.o10920(unit), 0x10 + rd8(game, 0x6d) * 4);
    if (e.o10174(sk, 0x13, 0) == 0 && e.o10174(sk, 0x13, 6) == 0 && e.o10174(sk, 0x13, 8) == 0 && e.o10174(sk, 0x13, 7) == 0 &&
        e.findQuest(unit, game, 0x20333367) == 0)
        e.o10003(sk, 0x13, 0xe);
    return 0;
}
}
