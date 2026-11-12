// 1.13c D2Game+0x62490 -- EAX in (upper half kept), ECX = quest record, EDX = event. Only for the event id 0x9B (+0x14, 16-bit) with a +0x18 word of 0 or 1: the D2Common import 0xA4C0 of the event's +0xC unit gives the player data, and the quest-flag buffer for the difficulty of the record's +4 game goes to the import 0xA57A(buffer, 0, 0), whose result is returned. Otherwise AX = the +0x18 word and the upper half of EAX is untouched.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f62490(GE& e, uint32_t eax, uint32_t q, uint32_t ev)
{
    const uint32_t w = e.rd16s(ev + 0x18) & 0xFFFF;
    if ((e.rd16s(ev + 0x14) & 0xFFFF) != 0x9b || w > 1) return (eax & 0xFFFF0000u) | w;
    const uint32_t game = e.rd32(q + 4);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ev + 0xc));
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    return e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0, 0);
}
}
