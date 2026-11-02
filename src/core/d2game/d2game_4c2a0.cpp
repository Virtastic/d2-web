// 1.13c D2Game+0x4C2A0: ecx = p1, edx = p2, stack idx and p4 (ret 8); `dt` = *sgptDataTables (+0xB98 rows of 0x23C, +0xBA0 count, +0xC4 and +0xBD4 limits).
// A valid row (+0x82 and +0x54 int16 in range) makes three 0xAC9A lookups (p2, row+0x60/+0x68/+0x64, idx, p4) and hands 0xA0C70 the block {idx, p4, res1, +0x82, +0x54, res2}.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4c2a0(uint32_t p1, uint32_t p2, int32_t idx, uint32_t p4, uint32_t dt, const D2Seam* e)
{
    if (idx < 0 || idx >= (int32_t)d2rd32(dt + 0xBA0)) return 0;
    const uint32_t row = (uint32_t)idx * 0x23C + d2rd32(dt + 0xB98);
    if (!row) return 0;
    const int32_t a = (int16_t)d2rd16(row + 0x82);
    if (a < 0 || a >= (int32_t)d2rd32(dt + 0xC4)) return 0;
    const int32_t b = (int16_t)d2rd16(row + 0x54);
    if (b < 0 || b >= (int32_t)d2rd32(dt + 0xBD4)) return 0;
    uint32_t blk[6];
    blk[0] = (uint32_t)idx; blk[1] = p4;
    blk[2] = e->call(0xAC9A, 0, 0, 0, 0, 0, p2, d2rd32(row + 0x60), (uint32_t)idx, p4);
    blk[3] = (uint32_t)a; blk[4] = (uint32_t)b;
    blk[5] = e->call(0xAC9A, 0, 0, 0, 0, 0, p2, d2rd32(row + 0x68), (uint32_t)idx, p4);
    const uint32_t res3 = e->call(0xAC9A, 0, 0, 0, 0, 0, p2, d2rd32(row + 0x64), (uint32_t)idx, p4);
    e->call(0xA0C70, p1, 0, 0, 0, 0, p2, 0, 0, res3);
    e->blk(0xA0C70, blk, 6);
    return 1;
}
