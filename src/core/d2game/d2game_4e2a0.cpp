// 1.13c D2Game+0x4E2A0 (MonsterSetStatsFromLevelTable): ecx = level, eax = unit, stack p1 (+0x6A byte, +0x6D byte, +0x74 dword), p2 target, p3 (ret 0xC).
// `dt` = *sgptDataTables (+0xB70 rows of 0x78 bytes, +0xB74 count). Callees: #10973 thunk 0xA42A, #10887 0xA472, #10551 0xA4D2.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4e2a0(int32_t lvl, uint32_t unit, uint32_t p1, uint32_t p2, int32_t p3, uint32_t dt, const D2Seam* e)
{
    if (lvl < 1) {
        const int32_t cur = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
        const int32_t x3 = (int32_t)((uint32_t)cur * 3u);
        int32_t v = ((int32_t)((uint32_t)x3 + (uint32_t)((x3 >> 31) & 3)) >> 2) + p3;
        if (v < 2) v = 1;
        lvl = v < cur ? v : cur;
        if (lvl == 0) goto after;
    }
    e->call(0xA472, 0, 0, 0, 0, 0, p2, 0xC, (uint32_t)lvl, 0);
after:
    {
        const int32_t count = (int32_t)d2rd32(dt + 0xB74);
        int32_t idx = count - 1;
        if (lvl < idx) idx = lvl;
        if (idx < 0 || idx >= count) return 0;
        const uint32_t row = d2rd32(dt + 0xB70) + (uint32_t)idx * 0x78;
        if (!row) return 0;
        int32_t a = (int32_t)d2rd8(p1 + 0x6D), edi = a <= 0 ? 0 : (a < 2 ? a : 2);
        const int32_t esi = (d2rd8(p1 + 0x6A) || d2rd32(p1 + 0x74)) ? 1 : 0;
        e->call(0xA4D2, 0, 0, 0, 0, 0, p2, 0x1F, d2rd32(row + (uint32_t)(edi + esi * 3) * 4), 0);
        e->call(0xA4D2, 0, 0, 0, 0, 0, p2, 0x13, d2rd32(row + (uint32_t)(edi + (esi + 2) * 3) * 4), 0);
        return 1;
    }
}
