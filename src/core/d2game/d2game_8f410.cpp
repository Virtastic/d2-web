// 1.13c D2Game+0x8F410 -- ECX unused, EDX = unit, one stack argument = pair {id, count}. The D2Common imports 0xA478(unit) then 0xA556 of its non-null answer give an id; when it equals the pair's id the count is incremented. Returns 0 when 0xA478 answers null, else the id.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f8f410(GE& e, uint32_t, uint32_t unit, uint32_t pair)
{
    const uint32_t p = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!p) return 0;
    const uint32_t id = e.h(0xA556, 0, 0, 0, 0, 0, p);
    if (e.rd32(pair) == id) e.wr32(pair + 4, e.rd32(pair + 4) + 1);
    return id;
}
}
