// 1.13c D2Game+0xE55C0 -- ECX = quest record, EDX = id, three stack arguments (the first = unit). AL = 1 only for id 0xf5 with 0x69590 (EAX = unit, ESI = id, one stack word = the quest's +4 field) answering 0; otherwise AL = 0. The upper bytes of EAX are the caller's, or the answer's once the callee ran.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe55c0(GE& e, uint32_t eax, uint32_t quest, uint32_t id, uint32_t unit, uint32_t, uint32_t)
{
    if (id != 0xf5) return eax & 0xFFFFFF00u;
    const uint32_t v = e.h(0x69590, unit, 0, 0, 0, id, e.rd32(quest + 4));
    return v ? v & 0xFFFFFF00u : 1;
}
}
