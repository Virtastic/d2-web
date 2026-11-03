// QuestA1Q4 setter 0x603D0 (EAX = game): the type-4 quest's data byte +0x92 := 1. No node: nothing.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x603D0
void d2game_603D0_q32(const Call&, uint32_t game) {
    uint32_t n = findQ(game, 4);
    if (n) wr8(rd32(n, 0x18), 0x92, 1);
}
