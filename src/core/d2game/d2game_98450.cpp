// 1.13c D2Game+0x98450 -- ECX = unit, EDX = state, one unused stack argument. The D2Common import 0xA3CA(unit, state, 0) runs; when 0xA68E(unit, 0x55) answers non-zero 0xA3CA(unit, 0x55, 0) follows. The helpers 0x9E370 (EBX = unit) and 0x9E6B0 (ESI = unit) end it; the last result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f98450(GE& e, uint32_t u, uint32_t st, uint32_t)
{
    e.h(0xA3CA, 0, 0, 0, 0, 0, u, st, 0);
    if (e.h(0xA68E, 0, 0, 0, 0, 0, u, 0x55)) e.h(0xA3CA, 0, 0, 0, 0, 0, u, 0x55, 0);
    e.h(0x9E370, 0, 0, 0, u, u);
    return e.h(0x9E6B0, 0, 0, 0, u, u);
}
}
