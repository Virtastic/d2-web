// 1.13c D2Game+0x9C000 -- ECX = a, EDX = unit, one stack argument = counter. A null unit returns 0. Otherwise 0xDFA90 (EAX = unit, ECX = a) is asked; a non-zero answer is returned, a zero answer increments the counter and returns its address.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f9c000(GE& e, uint32_t a, uint32_t unit, uint32_t counter)
{
    if (!unit) return 0;
    const uint32_t v = e.h(0xDFA90, unit, a, 0, 0, 0);
    if (v) return v;
    e.wr32(counter, e.rd32(counter) + 1);
    return counter;
}
}
