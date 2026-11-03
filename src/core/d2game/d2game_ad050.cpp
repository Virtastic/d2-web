// 1.13c D2Game+0xAD050: eax = skill id, edi = unit, stack game, a2 (ret 8). dt = *sgptDataTables, bm = gdwBitMasks (IAT 0xF8BE0).
// unit and 0 <= id < dt+0xBA0, row = dt+0xB98 + id*0x23C nonzero, row+4 & bm[5], state = s16 row+0x80 in [0, dt+0xC4):
// #A3CA(unit, state, 0); list = #A5D4(unit, state); list: #ABB6(unit, list), #ABAA(list). Then #8E900(eax = unit, ecx = game, ebx = a2; stack 9).
#include "d2game_seam4e.h"
extern "C" void d2g_ad050(uint32_t dt, uint32_t bm, uint32_t id, uint32_t unit, uint32_t game, uint32_t a2, const D2Seam* e)
{
    if (!unit || (int32_t)id < 0 || (int32_t)id >= (int32_t)d2rd32(dt + 0xBA0)) return;
    const uint32_t row = d2rd32(dt + 0xB98) + id * 0x23C;
    if (!row) return;
    if (!(d2rd32(bm + 0x14) & d2rd8(row + 4))) return;
    const int32_t st = (int16_t)d2rd16(row + 0x80);
    if (st < 0 || st >= (int32_t)d2rd32(dt + 0xC4)) return;
    e->call(0xA3CA, 0, 0, 0, 0, 0, unit, (uint32_t)st, 0);
    const uint32_t list = e->call(0xA5D4, 0, 0, 0, 0, 0, unit, (uint32_t)st);
    if (list) {
        e->call(0xABB6, 0, 0, 0, 0, 0, unit, list);
        e->call(0xABAA, 0, 0, 0, 0, 0, list);
    }
    e->call(0x8E900, unit, game, 0, a2, 0, 9);
}
