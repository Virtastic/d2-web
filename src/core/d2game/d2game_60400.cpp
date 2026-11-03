// 0x60400 (ECX = game, EDX = unit; plain ret): once per quest, read the unit's coordinates into the type-4 quest data at +0x9C
// (D2Common #10141, stack: unit, destination, ret 8) and set the done byte +0x67. 1 when it did, 0 when no node or already done.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x60400
uint32_t d2game_60400_q32(const Call& X, uint32_t game, uint32_t unit) {
    uint32_t n = findQ(game, 4);
    if (!n) return 0;
    uint32_t d = rd32(n, 0x18);
    if (rd8(d, 0x67)) return 0;
    X(0xA544, 0, 0, 0, 0, 0, unit, d + 0x9c);
    wr8(d, 0x67, 1);
    return 1;
}
