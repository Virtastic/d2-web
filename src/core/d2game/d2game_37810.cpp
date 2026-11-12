// 1.13c D2Game+0x37810 -- ECX unused, EDX = unit, one stack argument = counter. A null unit leaves EAX alone. A unit with bit 0 of its +0xC6 byte set returns 1. A player (type 0) in a mode other than 0 and 0x11, or a monster (type 1) in a mode other than 0 and 0xC, increments the counter and returns its address; those modes return 0, any other type returns type - 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f37810(GE& e, uint32_t eax, uint32_t, uint32_t unit, uint32_t counter)
{
    if (!unit) return eax;
    if (e.rd8(unit + 0xc6) & 1) return 1;
    const uint32_t type = e.rd32(unit), mode = e.rd32(unit + 0x10);
    if (type > 1) return type - 1;
    if (!mode || mode == (type ? 0xcu : 0x11u)) return 0;
    e.wr32(counter, e.rd32(counter) + 1);
    return counter;
}
}
