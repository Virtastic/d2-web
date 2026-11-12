// 1.13c D2Game+0xE7A10 -- ECX = quest record, EDX = event. Only when the event's +0x18 field is 0x4D: AL = the quest's +9 byte; a set byte with the quest's +0xC byte below 3 calls 0x35EB0 (EAX = 1, ESI = quest) and returns its result. Otherwise EAX keeps its upper bytes (all four when the event does not match).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe7a10(GE& e, uint32_t eax, uint32_t quest, uint32_t ev)
{
    if (e.rd32(ev + 0x18) != 0x4d) return eax;
    const uint32_t b = e.rd8(quest + 9);
    eax = (eax & 0xFFFFFF00u) | b;
    if (!b || e.rd8(quest + 0xc) >= 3) return eax;
    return e.h(0x35EB0, 1, 0, 0, 0, quest);
}
}
