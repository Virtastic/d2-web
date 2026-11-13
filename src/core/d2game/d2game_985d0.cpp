// 1.13c D2Game+0x985D0 -- state apply on a unit: a dead or absent unit first asks the state test; then sets the state, state 0x6B too on a live unit, and refreshes
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f985d0(NE& e, uint32_t u, uint32_t v, uint32_t s0)
{
    const bool live = (u && !(e.rd8(u + 0xc6) & 1) && ((e.rd32(u) == 0 && e.rd32(u + 0x10) != 0 && e.rd32(u + 0x10) != 0x11) || (e.rd32(u) == 1 && e.rd32(u + 0x10) != 0 && e.rd32(u + 0x10) != 0xc)));
    if (!live) { const uint32_t r = e.imp(0xA7E4, u, v); if (r) return r; }
    e.imp(0xA3CA, u, v, 0);
    if (live) e.imp(0xA3CA, u, 0x6b, 0);
    return e.imp(0xA59E, u, 0xB2000000u | 0xFBA14, 0x616);
}
}
