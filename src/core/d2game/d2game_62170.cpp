// 1.13c D2Game+0x62170 -- ECX = quest record, EDX = event. The id of the event's +0xC unit (its +0xC word; -1 for no unit) goes in EAX to the worker 0x33830 twice: first with ECX = the record +0x1C, then with ECX = the record's +0x18 data. The second result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f62170(GE& e, uint32_t q, uint32_t ev)
{
    uint32_t u = e.rd32(ev + 0xc);
    e.h(0x33830, u ? e.rd32(u + 0xc) : ~0u, q + 0x1c);
    u = e.rd32(ev + 0xc);
    return e.h(0x33830, u ? e.rd32(u + 0xc) : ~0u, e.rd32(q + 0x18));
}
}
