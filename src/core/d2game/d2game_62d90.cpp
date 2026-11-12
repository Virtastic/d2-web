// 1.13c D2Game+0x62D90 -- ECX = quest record: the fifteen words from +0xA0 are cleared, then +0xa0 = the callback 0x62410, +0xcc = the callback 0x62490, +0xe8 = the callback 0x60800, +0xec = the callback 0x607D0, +0xDC = the table at 0x1081C0, the +0xA byte = 1, the +0xB and +0xC bytes = 0, the +0x18 word = 0 and +0xE0 = 0x0. Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f62d90(GE& e, uint32_t q)
{
    for (uint32_t i = 0; i < 15; i++) e.wr32(q + 0xa0 + 4 * i, 0);
    e.wr32(q + 0xa0, 0xB2000000u | 0x62410);
    e.wr32(q + 0xcc, 0xB2000000u | 0x62490);
    e.wr32(q + 0xdc, 0xB2000000u | 0x1081c0);
    e.wr8(q + 0xa, 1);
    e.wr8(q + 0xc, 0);
    e.wr8(q + 0xb, 0);
    e.wr32(q + 0x18, 0);
    e.wr32(q + 0xe0, 0x0);
    e.wr32(q + 0xe8, 0xB2000000u | 0x60800);
    e.wr32(q + 0xec, 0xB2000000u | 0x607d0);
    return 0;
}
}
