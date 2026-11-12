// 1.13c D2Game+0xE41A0 -- ECX = quest record, EDX = event: 0xE660 (EAX = the event's +0xC unit, EBX = the event's +0 game, one stack word = the item code 0x20333367) is asked; a non-zero answer increments the +0x1c counter of the quest's +0x18 data. Then AL = the data's +0x20 flag byte; a set flag with the counter at 1 is cleared. EAX keeps the answer's upper bytes.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe41a0(GE& e, uint32_t quest, uint32_t ev)
{
    const uint32_t data = e.rd32(quest + 0x18);
    const uint32_t v = e.h(0xE660, e.rd32(ev + 0xc), 0, 0, e.rd32(ev), 0, 0x20333367);
    if (v) e.wr32(data + 0x1c, e.rd32(data + 0x1c) + 1);
    const uint32_t fl = e.rd8(data + 0x20);
    if (fl && e.rd32(data + 0x1c) == 1) e.wr8(data + 0x20, 0);
    return (v & 0xFFFFFF00u) | fl;
}
}
