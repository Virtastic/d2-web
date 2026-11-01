// 1.13c D2Game+0x50130: ecx = unit, edx = arg, one stack word (ret 4). A non-null unit: 0x37110(1, arg) with ebx = unit->+0x80; a player
// unit sends its +0x14 block's +0x28 word through 0x9D230 (ecx = unit->+0x80, edx = unit); then 0x9F7A0(unit, arg; stack word).
#include "d2game_seam4e.h"
extern "C" void d2game_50130(uint32_t unit, uint32_t arg, uint32_t a0, const D2Seam* e)
{
    if (!unit) return;
    const uint32_t u80 = d2rd32(unit + 0x80);
    e->call(0x37110, 0, 0, 0, u80, 0, 1, arg);
    if (d2rd32(unit) == 1) {
        const uint32_t pd = d2rd32(unit + 0x14);
        e->call(0x9D230, 0, u80, unit, 0, 0, pd ? d2rd32(pd + 0x28) : 0, 0);
    }
    e->call(0x9F7A0, 0, unit, arg, 0, 0, a0);
}
