// 1.13c D2Game+0x65410 -- ECX = quest record. AL returns 1 (upper bytes of EAX: the caller's, or those of the callee's answer). With the quest's +0xC byte clear and its +9 byte equal to 1, 0x33AF0 runs first (EBX = 0x14; stack: quest, the handler at 0x63D20).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f65410(GE& e, uint32_t eax, uint32_t quest)
{
    if (e.rd8(quest + 0xc) == 0 && e.rd8(quest + 9) == 1) eax = e.h(0x33AF0, 0, 0, 0, 0x14, 0, quest, 0xB2063D20u);
    return (eax & 0xFFFFFF00u) | 1;
}
}
