// 0x604C0 (EAX = game, ECX = out coords; plain ret): the type-4 quest's second stored point (data+0xA8/+0xAC) to out, 1, when the node's byte +9
// is set, data byte +0x96 is set and +0x92 is clear. Otherwise 0 and out untouched.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x604C0
uint32_t d2game_604C0_q32(const Call&, uint32_t game, uint32_t out) {
    uint32_t n = findQ(game, 4);
    if (!n || rd8(n, 9) == 0) return 0;
    uint32_t d = rd32(n, 0x18);
    if (rd8(d, 0x96) == 0 || rd8(d, 0x92) != 0) return 0;
    wr32(out, 0, rd32(d, 0xa8));
    wr32(out, 4, rd32(d, 0xac));
    return 1;
}
