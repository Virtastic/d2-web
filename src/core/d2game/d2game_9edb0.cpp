// 1.13c D2Game+0x9EDB0 -- nearest-unit visitor: after the optional filter, keeps the unit with the smallest squared distance to the search point
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f9edb0(NE& e, uint32_t ctx, uint32_t u)
{
    const uint32_t c = e.rd32(ctx + 0xc);
    if (e.rd32(c + 0x10) && !e.h(e.rd32(c + 0x10) & 0xffffffu, 0, e.rd32(ctx))) return 0;    // the caller's filter with ECX = its argument (not reached by the vectors)
    const uint32_t type = e.rd32(u), p = e.rd32(u + 0x2c);
    const bool fixed = type == 2 || type == 4 || type == 5;
    const uint32_t x = fixed ? e.rd32(p + 0xc) : p ? e.imp(0xA454, p) : 0;
    const uint32_t y = fixed ? e.rd32(p + 0x10) : p ? e.imp(0xA3B8, p) : 0;
    const int32_t dx = (int32_t)(x - e.rd32(c + 8)), dy = (int32_t)(y - e.rd32(c + 0xc));
    const int32_t d = (int32_t)((uint32_t)dy * (uint32_t)dy + (uint32_t)dx * (uint32_t)dx);
    if (d < (int32_t)e.rd32(c + 4)) { e.wr32(c + 4, (uint32_t)d); e.wr32(c, u); }
    return 1;
}
}
