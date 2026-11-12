// 1.13c D2Game+0xB97D0 -- ECX = quest record: the fifteen words from +0xA0 are cleared, then +0xa0 = the callback 0xB8C00, +0xe8 = the callback 0xB87D0, +0xec = the callback 0xB8B90, +0xDC = the table at 0x106FE8, the +0xA byte = 1, the +0xB and +0xC bytes = 0, the +0x18 word = 0 and +0xE0 = 0x21. Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fb97d0(GE& e, uint32_t q)
{
    for (uint32_t i = 0; i < 15; i++) e.wr32(q + 0xa0 + 4 * i, 0);
    e.wr32(q + 0xa0, 0xB2000000u | 0xb8c00);
    e.wr32(q + 0xdc, 0xB2000000u | 0x106fe8);
    e.wr8(q + 0xa, 1);
    e.wr8(q + 0xc, 0);
    e.wr8(q + 0xb, 0);
    e.wr32(q + 0x18, 0);
    e.wr32(q + 0xe0, 0x21);
    e.wr32(q + 0xe8, 0xB2000000u | 0xb87d0);
    e.wr32(q + 0xec, 0xB2000000u | 0xb8b90);
    return 0;
}
}
