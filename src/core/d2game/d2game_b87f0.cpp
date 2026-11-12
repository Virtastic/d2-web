// 1.13c D2Game+0xB87F0 -- ECX = quest record, EDX = event. The D2Common import 0xA532 is asked about the event's +8 item; the answer 0x20686668 ("hfh ") decrements the +0x10 counter of the quest's +0x18 data. Returns the answer.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fb87f0(GE& e, uint32_t quest, uint32_t ev)
{
    const uint32_t data = e.rd32(quest + 0x18);
    const uint32_t code = e.h(0xA532, 0, 0, 0, 0, 0, e.rd32(ev + 8));
    if (code == 0x20686668) e.wr32(data + 0x10, e.rd32(data + 0x10) - 1);
    return code;
}
}
