// 1.13c D2Game+0x42290 -- ECX = unit, EDX = state, one ignored stack argument: clears the state through the D2Common import 0xA3CA(unit, state, 0), then sets bit 0x40 of the unit's +0xC4 flags (a null unit is skipped). Returns the import's result.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f42290(GE& e, uint32_t unit, uint32_t state, uint32_t)
{
    const uint32_t v = e.h(0xA3CA, 0, 0, 0, 0, 0, unit, state, 0);
    if (unit) e.wr32(unit + 0xc4, e.rd32(unit + 0xc4) | 0x40);
    return v;
}
}
