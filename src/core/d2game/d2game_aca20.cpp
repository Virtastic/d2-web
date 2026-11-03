// 1.13c D2Game+0xACA20: item-use handler, ecx = game, edx = unit, stack item (+4 more, ret 0x14). dt = *sgptDataTables.
// The item's skill id (#A874) must be in [0, dt+0xBD4); the item's stat 0x47 (#A42A) nonzero: #A4D2 (unit, id, value, 0), return 1; else 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_aca20(uint32_t dt, uint32_t unit, uint32_t item, const D2Seam* e)
{
    const int32_t id = (int32_t)e->call(0xA874, 0, 0, 0, 0, 0, item);
    if (id < 0 || id >= (int32_t)d2rd32(dt + 0xBD4)) return 0;
    const uint32_t v = e->call(0xA42A, 0, 0, 0, 0, 0, item, 0x47, 0);
    if (!v) return 0;
    e->call(0xA4D2, 0, 0, 0, 0, 0, unit, (uint32_t)id, v, 0);
    return 1;
}
