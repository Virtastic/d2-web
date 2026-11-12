// 1.13c D2Game+0x56810 -- ECX = quest record, EDX = id, three stack arguments (the second = quest-flag buffer). AL = 0 unless the id is 0x203 and the quest's +0xC byte is 1; then the D2Common import 0xA568(buffer, quest +0xE0, 0) is asked and AL = 1 for a zero answer, 0 otherwise, the upper bytes of EAX being those of the negated answer.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f56810(GE& e, uint32_t eax, uint32_t quest, uint32_t id, uint32_t, uint32_t flags, uint32_t)
{
    if (id != 0x203 || e.rd8(quest + 0xc) != 1) return eax & 0xFFFFFF00u;
    const uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, flags, e.rd32(quest + 0xe0), 0);
    return ((0u - v) & 0xFFFFFF00u) | (v ? 0u : 1u);
}
}
