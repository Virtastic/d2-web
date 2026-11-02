// 1.13c D2Game+0x4E960: eax = idx, ecx = unit (64-bit seed at +0x20/+0x24), stack a0, a1 (ret 8); `dt` = *sgptDataTables (+0xB98 rows of 0x23C, +0xBA0 count).
// A unit with state 0x61 hands the 0x15F stat step to 0x4E390 (eax = flag, ecx = a0, res, 0); res = a1 > 2 and the seed's roll % 100 < row[+0x148].
#include "d2game_seam4e.h"
extern "C" void d2game_4e960(int32_t idx, uint32_t unit, uint32_t a0, int32_t a1, uint32_t dt, const D2Seam* e)
{
    uint32_t ebp = 1;
    if (e->call(0xA68E, 0, 0, 0, 0, 0, unit, 0x61)) {
        const uint32_t st = e->call(0xA5D4, 0, 0, 0, 0, 0, unit, 0x61);
        if (st) {
            e->call(0xA712, 0, 0, 0, 0, 0, st, 0x15e, 0);
            ebp = e->call(0xA712, 0, 0, 0, 0, 0, st, 0x15f, 0);
        }
    }
    uint32_t res = 0;
    if (a1 > 2) {
        int32_t chance = 0;
        if (idx >= 0 && idx < (int32_t)d2rd32(dt + 0xBA0)) {
            const uint32_t row = (uint32_t)idx * 0x23C + d2rd32(dt + 0xB98);
            if (row) chance = (int32_t)d2rd32(row + 0x148);
        }
        const uint64_t v = (uint64_t)d2rd32(unit + 0x20) * 0x6AC690C5ull + d2rd32(unit + 0x24);
        d2wr32(unit + 0x24, (uint32_t)(v >> 32));
        d2wr32(unit + 0x20, (uint32_t)v);
        res = (int32_t)((uint32_t)v % 100u) < chance ? 1 : 0;
    }
    e->call(0x4E390, ebp, a0, 0, 0, 0, res, 0);
}
