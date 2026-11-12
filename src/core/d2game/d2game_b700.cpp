// 1.13c D2Game+0xB700 -- ECX = game, EDX = index, two stack arguments x and y. The table is the word at +0x48 + 4 * index of the game's +0x10F0 block; its +0x18 word counts the (x, y) pairs from +0x20. AL = 0 as soon as a pair is closer than 0x64 to the arguments on either axis, AL = 1 when none is; the upper three bytes of EAX are those of the last distance computed (of the table pointer when the count is not positive).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fb700(GE& e, uint32_t game, uint32_t idx, uint32_t x, uint32_t y)
{
    const uint32_t t = e.rd32(e.rd32(game + 0x10f0) + idx * 4 + 0x48);
    const int32_t n = (int32_t)e.rd32(t + 0x18);
    uint32_t a = t;
    for (int32_t i = 0; i < n; i++) {
        a = e.rd32(t + 0x20 + 8 * i) - x;
        if ((int32_t)a < 0) a = 0u - a;
        if ((int32_t)a < 0x64) return a & ~0xffu;
        a = e.rd32(t + 0x24 + 8 * i) - y;
        if ((int32_t)a < 0) a = 0u - a;
        if ((int32_t)a < 0x64) return a & ~0xffu;
    }
    return (a & ~0xffu) | 1;
}
}
