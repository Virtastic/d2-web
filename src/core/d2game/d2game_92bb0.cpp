// 1.13c D2Game+0x92BB0: ebx = row index, stack a0..a5 (ret 0x18); a1 = unit. sum = 0xACD6(a1, idx, a4) + 0xA42A(a1, 0x145, 0); a 0x70-byte
// request block (zeroed) takes the word 0xDE5A0(eax = a2; a0, sum, 0) at +4. When its bit 0 is set: a5 ORs 0xC into the first byte, the count at +0xC is
// row.+0x148 + (a4 - 1) * row.+0x14C for a4 > 0 on a skills row (dt+0xB98, stride 0x23C, count dt+0xBA0) else 0, and 0x92360(eax = a1; a0, &block, a2, idx, a4) runs.
// Always +0x60 = 1, then 0xDDDE0(eax = a1, cl = byte a3+0x1A5 or 0x80, edi = &block; a0, a2) closes it. dt = *sgptDataTables.
#include "d2game_seam4e.h"
extern "C" void d2game_92bb0(uint32_t idx, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t dt, const D2Seam* e)
{
    uint32_t q[28] = {};
    const uint32_t x = e->call(0xACD6, 0, 0, 0, 0, 0, a1, idx, a4);
    const uint32_t y = e->call(0xA42A, 0, 0, 0, 0, 0, a1, 0x145, 0);
    const uint32_t ax = e->call(0xDE5A0, a2, 0, 0, 0, 0, a0, x + y, 0) & 0xFFFF;
    q[1] = ax;
    if (ax & 1) {
        if (a5) q[1] |= 0xC;
        uint32_t row = 0, cnt = 0;
        if ((int32_t)idx >= 0 && (int32_t)idx < (int32_t)d2rd32(dt + 0xBA0)) row = idx * 0x23C + d2rd32(dt + 0xB98);
        if (row && (int32_t)a4 > 0) cnt = d2rd32(row + 0x148) + (a4 - 1) * d2rd32(row + 0x14C);
        q[3] = cnt;
        e->call(0x92360, a1, 0, 0, 0, 0, a0, (uint32_t)(uintptr_t)q, a2, idx, a4);
        e->blk(0x92360, q, 28);
    }
    q[24] = 1;
    uint32_t cl = d2rd8(a3 + 0x1A5);
    if (!cl) cl = 0x80;
    e->call(0xDDDE0, a1, cl, 0, 0, 0, a0, a2);
    e->blk(0xDDDE0, q, 9);
}
