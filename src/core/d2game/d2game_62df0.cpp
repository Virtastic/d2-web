// 1.13c D2Game+0x62DF0 -- ECX = quest record, EDX = id, three stack arguments (the first a unit). The quest-flag buffer is the unit's player data buffer for the record's +4 game difficulty. AL = 1 only when the import 0xA568(buffer, 1, 0) does not answer 1, the id is 0x94 and the helper 0x69590 (EAX = unit, ESI = id, stack = the game) answers 0; AL = 0 otherwise, over the upper bytes of the last answer.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f62df0(GE& e, uint32_t q, uint32_t id, uint32_t unit, uint32_t, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(e.rd32(q + 4) + 0x6d));
    uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x1, 0x0);
    if (v == 1 || id != 0x94) return v & ~0xffu;
    v = e.h(0x69590, unit, 0, 0, 0, id, e.rd32(q + 4));
    if (v) return v & ~0xffu;
    return 1;
}
}
