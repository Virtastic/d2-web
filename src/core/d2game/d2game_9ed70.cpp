// 1.13c D2Game+0x9ED70 -- ECX = record whose +0xC points at a tracker {best unit, best id, low unit, low id, floor, count}; EDX = unit (id at +0xC, -1 for none). An id above the floor and below the best id becomes the new best; otherwise an id not above the low id becomes the new low. The count is incremented either way; returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f9ed70(GE& e, uint32_t rec, uint32_t unit)
{
    const uint32_t t = e.rd32(rec + 0xc), id = unit ? e.rd32(unit + 0xc) : ~0u;
    if (id > e.rd32(t + 0x10) && id < e.rd32(t + 4)) { e.wr32(t + 4, id); e.wr32(t, unit); }
    else if (id <= e.rd32(t + 0xc)) { e.wr32(t + 0xc, id); e.wr32(t + 8, unit); }
    e.wr32(t + 0x14, e.rd32(t + 0x14) + 1);
    return 1;
}
}
