// 1.13c D2Game+0xC2E20 -- EAX = whatever the caller left, ECX = quest record, EDX = event. A zero +9 record byte returns EAX with AL = 0. An event +0x18 value below 0x28 (signed), an event +0x14 id other than 0x28 or a zero +0x9C record word returns 0x28. Otherwise the result of the helper 0x33830 is returned (EAX = the +0xC id of the event's +0xC unit, or -1 without one; ECX = record + 0x1C).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc2e20(GE& e, uint32_t a, uint32_t q, uint32_t ev)
{
    if (!e.rd8(q + 9)) return a & ~0xffu;
    if ((int32_t)e.rd32(ev + 0x18) < 0x28 || e.rd32(ev + 0x14) != 0x28 || !(e.rd32(q + 0x9c) & 0xffff)) return 0x28;
    const uint32_t u = e.rd32(ev + 0xc);
    return e.h(0x33830, u ? e.rd32(u + 0xc) : 0xffffffffu, q + 0x1c, 0, 0, 0);
}
}
