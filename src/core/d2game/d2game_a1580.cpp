// 1.13c D2Game+0xA1580: eax = flag, ecx = unit, stack a0 (ret 4). unit+0xC8 &= ~0x40; flag && #D8440(a0, unit) runs #DFFD0(4, unit);
// client id = (unit && unit type 0) ? [#A4C0(unit) + 0x9C] : 0; returns #8A3E0(eax = client; stack &block, 8) over the 8-byte block 3F FF 00 00 00 00 FF FF.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_a1580(uint32_t flag, uint32_t unit, uint32_t a0, const D2Seam* e)
{
    if (unit) d2wr32(unit + 0xC8, d2rd32(unit + 0xC8) & ~0x40u);
    if (flag && e->call(0xD8440, 0, 0, 0, 0, 0, a0, unit)) e->call(0xDFFD0, 0, 0, 0, 0, 0, 4, unit);
    uint32_t cl = 0;
    if (unit && d2rd32(unit) == 0) cl = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C);
    uint32_t blk[2] = { 0x0000FF3Fu, 0xFFFF0000u };
    const uint32_t r = e->call(0x8A3E0, cl, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, 8);
    e->blk(0x8A3E0, blk, 2);
    return r;
}
