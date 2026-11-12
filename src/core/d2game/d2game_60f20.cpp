// 1.13c D2Game+0x60F20 -- ECX = quest record, EDX = id, three stack arguments (the first = unit). The unit's id (+0xC, -1 for none) is searched in the id list of the quest's +0x18 data (count: 16-bit +0x64, ids from +0x34). AL = 1 when it is found and the id argument is neither 0x9B nor 0x93; otherwise AL = 0. The upper bytes of EAX are those of the index reached.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f60f20(GE& e, uint32_t quest, uint32_t id, uint32_t unit, uint32_t, uint32_t)
{
    const uint32_t data = e.rd32(quest + 0x18), uid = unit ? e.rd32(unit + 0xc) : ~0u, n = e.rd16s(data + 0x64) & 0xFFFF;
    uint32_t i = 0;
    for (; i < n; ++i) if (e.rd32(data + 0x34 + 4 * i) == uid) break;
    if (i == n || id == 0x9b || id == 0x93) return i & 0xFFFFFF00u;
    return (i & 0xFFFFFF00u) | 1;
}
}
