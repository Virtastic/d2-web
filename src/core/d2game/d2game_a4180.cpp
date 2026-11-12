// 1.13c D2Game+0xA4180 -- ECX unused, EDX = a, two stack arguments (the first = unit). 0xC110 (ESI = unit) answering 0 returns 0. Otherwise 0xB0A20 (EBX = a, one stack word = unit) rates the unit: a rating of at most 0x19 (unsigned) returns the unit, a larger one 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fa4180(GE& e, uint32_t, uint32_t a, uint32_t unit, uint32_t)
{
    if (!e.h(0xC110, 0, 0, 0, 0, unit)) return 0;
    return e.h(0xB0A20, 0, 0, 0, a, 0, unit) > 0x19 ? 0 : unit;
}
}
