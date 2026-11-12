// 1.13c D2Game+0x618D0 -- ECX = quest record, EDX = event: the worker 0xE660 (EAX = the event's +0xC unit, EBX = the event's +0 game) is asked for the item codes 0x20646B62 and 0x20736B62; each non-zero answer increments the +0x7C counter of the record's +0x18 data. Returns the second answer.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f618d0(GE& e, uint32_t q, uint32_t ev)
{
    const uint32_t d = e.rd32(q + 0x18);
    const uint32_t a = e.h(0xE660, e.rd32(ev + 0xc), 0, 0, e.rd32(ev), 0, 0x20646b62);
    const uint32_t b = e.h(0xE660, e.rd32(ev + 0xc), 0, 0, e.rd32(ev), 0, 0x20736b62);
    if (a) e.wr32(d + 0x7c, e.rd32(d + 0x7c) + 1);
    if (b) e.wr32(d + 0x7c, e.rd32(d + 0x7c) + 1);
    return b;
}
}
