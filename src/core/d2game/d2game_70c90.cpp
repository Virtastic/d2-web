// 1.13c D2Game+0x70C90 -- EDX = unit, one stack argument (a second unit, may be null). Equal units return the argument. Otherwise the six-byte packet 0x92 (0, then the second unit's +0xC id or -1) is sent through 0x8A3E0 with EAX = the +0x9C client of the first unit's player data (D2Common import 0xA4C0) when it is a player, 0 otherwise; the sender's result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f70c90(GE& e, uint32_t, uint32_t u, uint32_t o)
{
    if (u == o) return o;
    uint32_t c = 0;
    if (u && e.rd32(u) == 0) c = e.rd32(e.h(0xa4c0, 0, 0, 0, 0, 0, u) + 0x9c);
    return e.h(0x8A3E0, c, 0, 0, 0, 0, 0xB3000000u, 6);
}
}
