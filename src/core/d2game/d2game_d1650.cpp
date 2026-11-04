// D2Game+0xD1650 -- EDI = unit, ESI = item, plain ret (no stack arguments). Written from the 1.13c disassembly; callees are the D2Seam.
// A null unit or item asserts (0x19A1 / 0x19A2) and exits: not modelled. Otherwise: 0xA4A2(item, 4), 0xA916(item, 0x100000, 1),
// 0xA994([unit + 0x60], item), 0xA9A6(unit, 1); returns 1.
#include "d2game_seam4e.h"
// 1.13c D2Game+0xD1650
extern "C" uint32_t d2game_d1650(uint32_t unit, uint32_t item, const D2Seam* e)
{
    e->call(0xA4A2, 0, 0, 0, 0, 0, item, 4);
    e->call(0xA916, 0, 0, 0, 0, 0, item, 0x100000, 1);
    e->call(0xA994, 0, 0, 0, 0, 0, d2rd32(unit + 0x60), item);
    e->call(0xA9A6, 0, 0, 0, 0, 0, unit, 1);
    return 1;
}
