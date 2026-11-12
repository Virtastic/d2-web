// 1.13c D2Game+0xA38C0 -- ECX and EDX = two units (never null): 1 when the level of both units' rooms (D2Common imports 0xA478 then 0xA556 on each) is the same, 0 otherwise.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fa38c0(GE& e, uint32_t u1, uint32_t u2)
{
    const uint32_t l1 = e.h(0xA556, 0, 0, 0, 0, 0, e.h(0xa478, 0, 0, 0, 0, 0, u1));
    const uint32_t l2 = e.h(0xA556, 0, 0, 0, 0, 0, e.h(0xa478, 0, 0, 0, 0, 0, u2));
    return l1 == l2 ? 1 : 0;
}
}
