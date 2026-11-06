// 1.13c D2Game+0xADB30: edi = unit, stack game, a1, skillId, a3 (ret 0x10); void.
// dt = *sgptDataTables, bm = gdwBitMasks (IAT 0xF8BE0). unit and 0 <= skillId < dt+0xBA0, row = dt+0xB98 + id*0x23C nonzero,
// row+4 & bm[0x14], state = s16 row+0x80 in [0, dt+0xC4). Then #8E900(eax unit, ecx game, ebx a1; 9), d = #9E970(eax a3, ecx id,
// edx row; game, unit), #8EA80(ecx game, edx unit; 9, d, 0, a1, id) and, when row+5 & bm[0x1C], #A19A0(eax unit; game, id, a3, 1, 1, 0).
#include "d2game_seam4e.h"
extern "C" void d2g_adb30(uint32_t dt, uint32_t bm, uint32_t unit, uint32_t game, uint32_t a1, uint32_t skill, uint32_t a3, const D2Seam* e)
{
    if (!unit || (int32_t)skill < 0 || (int32_t)skill >= (int32_t)d2rd32(dt + 0xBA0)) return;
    const uint32_t row = d2rd32(dt + 0xB98) + skill * 0x23C;
    if (!row) return;
    if (!(d2rd8(bm + 0x14) & d2rd8(row + 4))) return;
    const int32_t st = (int16_t)d2rd16(row + 0x80);
    if (st < 0 || st >= (int32_t)d2rd32(dt + 0xC4)) return;
    e->call(0x8E900, unit, game, 0, a1, 0, 9);
    const uint32_t d = e->call(0x9E970, a3, skill, row, 0, 0, game, unit);
    const uint32_t w[5] = { 9, d, 0, a1, skill };
    e->call(0x8EA80, 0, game, unit, 0, 0, w[0], w[1], w[2], w[3], w[4]);
    e->blk(0x8EA80, w, 5);
    if (d2rd8(bm + 0x1C) & d2rd8(row + 5)) {
        const uint32_t v[6] = { game, skill, a3, 1, 1, 0 };
        e->call(0xA19A0, unit, 0, 0, 0, 0, v[0], v[1], v[2], v[3], v[4], v[5]);
        e->blk(0xA19A0, v, 6);
    }
}
