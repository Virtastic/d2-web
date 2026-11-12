// 1.13c D2Game+0xBA810 -- EAX = whatever the caller left, ECX = quest record, EDX = event. A zero +9 record byte returns EAX with AL = 0. Otherwise the +0xB8 word of the event's +8 unit becomes 0x20686668 and the helper 0x11A70 runs (EBX = the event's +0 game; stack = the unit, 7, a local, 0, -1, 0); a non-zero result increments the +0x10 word of the record's +0x18 data. The helper's result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fba810(GE& e, uint32_t a, uint32_t q, uint32_t ev)
{
    if (!e.rd8(q + 9)) return a & ~0xffu;
    const uint32_t d = e.rd32(q + 0x18), u = e.rd32(ev + 8);
    e.wr32(u + 0xb8, 0x20686668);
    const uint32_t v = e.hs(0x11A70, 0, 0, 0, e.rd32(ev), 0, {u, 7, 0xB3000000u, 0, 0xffffffffu, 0});
    if (v) e.wr32(d + 0x10, e.rd32(d + 0x10) + 1);
    return v;
}
}
