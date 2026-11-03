// 1.13c D2Game+0xA1D10: A1D10 spawn-item-on-unit helper.
// idx (stack arg1) in [0, dt+0xBA0); row = dt+0xB98 + idx*0x23C; bm[5] & row[4], short row+0x80 in [0, dt+0xC4); v = A3F4(edi unit, idx); v nonzero; cnt = A400(unit, v, 1) > 0. If bm[7] & row[5]: A19A0(unit; arg0, idx, cnt, 1, 0, 0), 9F3B0(idx, arg0; unit, cnt, 1). Else A3CA(unit, s, 1); it = A5D4(unit, s) or A6A0(unit[8] | 0, 0, 0, unit[0] | 6, unit[0xC] | -1); on an item A5B6(it, s), AE3E(it, arg0), AE50(it, cnt), A60A(unit, it, 1), A6E2(it, 0x15E, arg0, 0), A6E2(it, 0x15F, cnt, 0); then 9F3B0(idx, arg0; unit, cnt, 1).
#include "d2game_seam4e.h"
extern "C" void d2g_a1d10(uint32_t unit, uint32_t arg0, int32_t idx, uint32_t dt, uint32_t bm, const D2Seam* e)
{
    if (idx < 0 || idx >= (int32_t)d2rd32(dt + 0xBA0)) return;
    const uint32_t row = (uint32_t)idx * 0x23C + d2rd32(dt + 0xB98);
    if (!row) return;
    if (!(d2rd8(bm + 0x14) & d2rd8(row + 4))) return;
    const int16_t sh = (int16_t)d2rd16(row + 0x80);
    if (sh < 0 || sh >= (int32_t)d2rd32(dt + 0xC4)) return;
    const uint32_t v = e->call(0xA3F4, 0, unit, (uint32_t)idx);
    if (!v) return;
    const int32_t cnt = (int32_t)e->call(0xA400, 0, 0, 0, 0, 0, unit, v, 1);
    if (cnt <= 0) return;
    if (d2rd8(bm + 0x1C) & d2rd8(row + 5)) {
        e->call(0xA19A0, unit, 0, 0, 0, 0, arg0, (uint32_t)idx, (uint32_t)cnt, 1, 0);
        e->call(0x9F3B0, (uint32_t)idx, arg0, 0, 0, 0, unit, (uint32_t)cnt, 1);
        return;
    }
    e->call(0xA3CA, 0, 0, 0, 0, 0, unit, (uint32_t)(int32_t)sh, 1);
    uint32_t it = e->call(0xA5D4, 0, 0, 0, 0, 0, unit, (uint32_t)(int32_t)sh);
    if (!it) {
        const uint32_t x = unit ? d2rd32(unit + 8) : 0, ty = unit ? d2rd32(unit) : 6, y = unit ? d2rd32(unit + 0xC) : 0xFFFFFFFFu;
        it = e->call(0xA6A0, 0, 0, 0, 0, 0, x, 0, 0, ty, y);
    }
    if (it) {
        e->call(0xA5B6, 0, 0, 0, 0, 0, it, (uint32_t)(int32_t)sh);
        e->call(0xAE3E, 0, 0, 0, 0, 0, it, arg0);
        e->call(0xAE50, 0, 0, 0, 0, 0, it, (uint32_t)cnt);
        e->call(0xA60A, 0, 0, 0, 0, 0, unit, it, 1);
        e->call(0xA6E2, 0, 0, 0, 0, 0, it, 0x15E, arg0, 0);
        e->call(0xA6E2, 0, 0, 0, 0, 0, it, 0x15F, (uint32_t)cnt, 0);
    }
    e->call(0x9F3B0, (uint32_t)idx, arg0, 0, 0, 0, unit, (uint32_t)cnt, 1);
}
