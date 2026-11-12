// 1.13c D2Game+0x62FA0 -- ECX = quest record: the fifteen words from +0xA0 are cleared, then +0xa0 = the callback 0x624D0, +0xc0 = the callback 0x60180, +0xe8 = the callback 0x60170, +0xec = the callback 0x60820, +0xDC = the table at 0x10A510, the +0xA byte = 1, the +0xB and +0xC bytes = 0 and +0xE0 = 0x1d. Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f62fa0(GE& e, uint32_t q)
{
    for (uint32_t i = 0; i < 15; i++) e.wr32(q + 0xa0 + 4 * i, 0);
    e.wr32(q + 0xa0, 0xB2000000u | 0x624d0);
    e.wr32(q + 0xc0, 0xB2000000u | 0x60180);
    e.wr32(q + 0xdc, 0xB2000000u | 0x10a510);
    e.wr8(q + 0xa, 1);
    e.wr8(q + 0xc, 0);
    e.wr8(q + 0xb, 0);
    e.wr32(q + 0xe0, 0x1d);
    e.wr32(q + 0xe8, 0xB2000000u | 0x60170);
    e.wr32(q + 0xec, 0xB2000000u | 0x60820);
    return 0;
}
}
