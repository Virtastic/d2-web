// 1.13c D2Common+0x7C570 -- keyword callback (ecx = text): compares the text through Storm 509 (at most 32 characters) with "min", "max", "rand", "skill" and "miss" in that order and answers the position of the first match, or -1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
uint32_t f7c570(GE& e, uint32_t s)
{
    static const uint32_t kw[5] = { 0xB208b450u, 0xB208b44cu, 0xB208b444u, 0xB208b43cu, 0xB208b434u };
    for (uint32_t k = 0; k < 5; k++)
        if (!e.h(0x9204, 0, 0, 0, 0, 0, s, kw[k], 0x20)) return k;
    return 0xffffffffu;
}
}
