// 0x60450 (ECX = game, EDX = unit, stack = out coords; ret 4): the type-4 quest's stored point. Until the byte +0x95 is set the unit's own
// coordinates go to out (D2Common #10141); afterwards the stored pair at data+0x9C/+0xA0 is copied to out. 1 with a node, else 0.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x60450
uint32_t d2game_60450_q32(const Call& X, uint32_t game, uint32_t unit, uint32_t out) {
    uint32_t n = findQ(game, 4);
    if (!n) return 0;
    uint32_t d = rd32(n, 0x18);
    if (rd8(d, 0x95) == 0) {
        X(0xA544, 0, 0, 0, 0, 0, unit, out);
        return 1;
    }
    wr32(out, 0, rd32(d, 0x9c));
    wr32(out, 4, rd32(d, 0xa0));
    return 1;
}
