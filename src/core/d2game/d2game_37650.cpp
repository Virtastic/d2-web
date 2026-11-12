// 1.13c D2Game+0x37650 -- ECX unused, EDX = unit, one stack argument = pair {id, count}. The D2Common imports 0xA478(unit) and 0xA556 of its answer give an id; when it equals the pair's id and the unit is non-null with a +0x10 mode that is neither 0 nor 0x11, the count is incremented. Returns the id.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f37650(GE& e, uint32_t, uint32_t unit, uint32_t pair)
{
    const uint32_t id = e.h(0xA556, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, unit));
    if (id != e.rd32(pair) || !unit) return id;
    const uint32_t mode = e.rd32(unit + 0x10);
    if (mode && mode != 0x11) e.wr32(pair + 4, e.rd32(pair + 4) + 1);
    return id;
}
}
