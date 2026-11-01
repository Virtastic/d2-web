// 1.13c D2Game+0x4EDD0: edx = unit, two ignored stack words (ret 8). Resolves the unit owner (#A598 of unit->+0x2C); an owner of type 4, subtype 3 that passes #B1B6 and #A8A4 returns true when #A952(owner, 0) is 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4edd0(uint32_t unit, const D2Seam* e)
{
    e->call(0xE07A0, unit);
    const uint32_t v = e->call(0xA598, 0, 0, 0, 0, 0, d2rd32(unit + 0x2C));
    if (v == unit || !v) return 0;
    if (d2rd32(v) != 4 || d2rd32(v + 0x10) != 3) return 0;
    if (!e->call(0xB1B6, 0, 0, 0, 0, 0, d2rd32(v + 4))) return 0;
    if (!e->call(0xA8A4, 0, 0, 0, 0, 0, v, 0x10, 0x100, 0xB20FC1DCu)) return 0;
    return e->call(0xA952, 0, 0, 0, 0, 0, v, 0) == 0;
}
