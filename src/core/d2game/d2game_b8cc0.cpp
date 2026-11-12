// 1.13c D2Game+0xB8CC0 -- ECX = quest record, EDX = event: 0xE660 (EAX = the event's +0xC unit, EBX = the event's +0 game, one stack word = the item code 0x20686668) is asked; a non-zero answer increments the +0x10 counter of the quest's +0x18 data. Returns the answer.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fb8cc0(GE& e, uint32_t quest, uint32_t ev)
{
    const uint32_t data = e.rd32(quest + 0x18);
    const uint32_t v = e.h(0xE660, e.rd32(ev + 0xc), 0, 0, e.rd32(ev), 0, 0x20686668);
    if (v) e.wr32(data + 0x10, e.rd32(data + 0x10) + 1);
    return v;
}
}
