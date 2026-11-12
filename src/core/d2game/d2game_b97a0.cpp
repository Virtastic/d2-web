// 1.13c D2Game+0xB97A0 -- ECX unused, EDX = event. Only when the event's 16-bit +0x14 is 0x16f and its 16-bit +0x18 is 0x298: the player data of the event's +0xC unit (import 0xA4C0) gives the quest-flag buffer for the game's difficulty, which goes to the import 0xA57A(buffer, 0x18, 0); its result is returned. Otherwise EAX is not touched.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fb97a0(GE& e, uint32_t eax, uint32_t, uint32_t ev)
{
    if ((e.rd16s(ev + 0x14) & 0xFFFF) != 0x16f || (e.rd16s(ev + 0x18) & 0xFFFF) != 0x298) return eax;
    const uint32_t game = e.rd32(ev);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ev + 0xc));
    return e.h(0xA57A, 0, 0, 0, 0, 0, e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d)), 0x18, 0);
}
}
