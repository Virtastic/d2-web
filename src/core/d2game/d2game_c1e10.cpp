// 1.13c D2Game+0xC1E10 -- EAX = whatever the caller left (only AL is the result; the upper bytes come back from the last value computed), ECX = quest record, EDX = id, three stack arguments. For the ids 0xAF, 0xB0, 0xB1, 0xB2, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2 and 0xF4 the import 0xA568(second stack argument, 0xD, 1) is asked: AL = 1 when it answers exactly 1. AL = 0 otherwise.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc1e10(GE& e, uint32_t a, uint32_t q, uint32_t id, uint32_t s0, uint32_t s1, uint32_t)
{
    switch (id) {
    case 0xaf: case 0xb0: case 0xb1: case 0xb2: case 0xc7: case 0xc8: case 0xc9: case 0xca: case 0xd2: case 0xf4: break;
    default: return a & ~0xffu;
    }
    const uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, s1, 0xd, 0x1);
    return v == 1 ? 1 : v & ~0xffu;
}
}
