// 1.13c D2Game+0x50DB0 -- timer callback: checks its target, marks the monster record and queues the next step
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f50db0(NE& e, uint32_t unit, uint32_t p)
{
    if (!e.h(0x4EE80, 0, 0, 0, 0, unit, e.rd32(p), e.rd32(p + 4))) return 0;
    const uint32_t v = e.rd32(p + 0x14);
    if (unit && e.rd32(unit) == 1 && e.h(0x15D0, e.rd32(unit + 4), 0, 0x10)) {
        const uint32_t m = e.rd32(unit + 0x14);
        if (m) { e.wr32(m + 0x38, e.rd8(p + 0x10) ? 2 : 1); e.wr32(m + 0x34, v); }
    }
    const uint32_t g = e.rd32(p);
    e.h(0x8EA80, 0, g, unit, 0, 0, 0xa, e.rd32(g + 0xa8) + e.rd32(p + 0x18), 0, 0, 0);
    return 1;
}
}
