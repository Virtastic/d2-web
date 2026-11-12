// 1.13c D2Game+0x61BE0 -- ECX = quest record, EDX = event: tail-calls 0x33830 with EAX = the id (+0xC) of the event's +0xC unit (-1 without one) and ECX = the quest's +0x18 data +0x14.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f61be0(GE& e, uint32_t quest, uint32_t ev)
{
    const uint32_t t = e.rd32(ev + 0xc);
    return e.h(0x33830, t ? e.rd32(t + 0xc) : ~0u, e.rd32(quest + 0x18) + 0x14, 0, 0, 0);
}
}
