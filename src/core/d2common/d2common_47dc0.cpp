// 1.13c D2Common+0x47DC0 -- seven stack arguments, ret 28 (first = item unit, second = property record; a NULL record is fatal, line 0x312): when the unit is an item (type 4) with item data at +0x14, sets bit 0x800 of the item flags at +0x18; then calls 0x2A3D0 with the unit and the dword at +4 of the record. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
uint32_t f47dc0(GE& e, uint32_t u, uint32_t rec)
{
    if (!rec) {
        const uint32_t v = e.h(0x921c);
        e.h(0x9216, 0, 0, 0, 0, 0, 0xB208a728u, v, 0x312);
        e.h(0x1b0d, 0, 0, 0, 0, 0, 0xffffffffu);
        return 1;
    }
    if (u && e.rd32(u) == 4) {
        const uint32_t d = e.rd32(u + 0x14);
        if (d) e.wr32(d + 0x18, e.rd32(d + 0x18) | 0x800);
    }
    e.h(0x2a3d0, 0, 0, 0, 0, 0, u, e.rd32(rec + 4));
    return 1;
}
}
