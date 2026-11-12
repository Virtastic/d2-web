// 1.13c D2Game+0x359C0 -- ECX = game, EDX = unit, one unused stack argument. A two-byte packet {0x89, the +0x20 byte of the game's +0x10F4 record} is built; the helper 0x34C00 runs (EAX = unit, stack = game), then the packet goes through 0x8A3E0 with EAX = the +0x9C client of the unit's player data (D2Common import 0xA4C0) for a player unit (type 0), 0 otherwise. Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f359c0(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pkt[1] = {0x89u | ((uint32_t)e.rd8(e.rd32(game + 0x10f4) + 0x20) << 8)};
    e.h(0x34C00, unit, 0, 0, 0, 0, game);
    uint32_t c = 0;
    if (unit && e.rd32(unit) == 0) c = e.rd32(e.h(0xa4c0, 0, 0, 0, 0, 0, unit) + 0x9c);
    e.hs(0x8A3E0, c, 0, 0, 0, 0, {e.stk(pkt), 2}, 0, 1);
    return 0;
}
}
