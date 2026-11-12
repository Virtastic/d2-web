// 1.13c D2Game+0xE3E00 -- ECX = quest record, EDX = event. Only when the event's +0x14 field is 0x4b and the quest's 16-bit +0x9C count is non-zero: tail-calls 0x33830 with EAX = the id of the event's +0xC unit (-1 without one) and ECX = the quest record +0x1C. Otherwise EAX is not touched.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe3e00(GE& e, uint32_t eax, uint32_t quest, uint32_t ev)
{
    if (e.rd32(ev + 0x14) != 0x4b || (e.rd16s(quest + 0x9c) & 0xFFFF) == 0) return eax;
    const uint32_t t = e.rd32(ev + 0xc);
    return e.h(0x33830, t ? e.rd32(t + 0xc) : ~0u, quest + 0x1c, 0, 0, 0);
}
}
