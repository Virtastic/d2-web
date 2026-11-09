// 1.13c D2Game+0xCA410 -- release of two unit slot refs. EAX = slot table, EDX = slot A, EBX = skip flag, stack s0 = slot B, s1, s2, s3 = unit (ret 0x10).
// Written from the 1.13c disassembly. The one callee 0xA514 (#10830) takes the unit as a stack word.
#include "d2game_seam4e.h"
extern "C" void d2game_ca410(uint32_t tbl, uint32_t slotA, uint32_t ebx, uint32_t slotB, uint32_t p4, uint32_t p5, uint32_t unit, const D2Seam* e)
{
    const uint32_t a = d2rd32(tbl + slotA * 4), b = d2rd32(tbl + slotB * 4);
    if (!unit) {
        if (!ebx && !p5 && !p4) d2wr32(a + 0x2CC, d2rd32(a + 0x2CC) - 1);
        return;
    }
    if (a != b && d2rd32(unit) == 1 && d2rd32(unit + 0x14)) d2wr32(d2rd32(unit + 0x14) + 0x58, slotA);
    if (ebx) return;
    if (e->call(0xA514, 0, 0, 0, 0, 0, unit) != 0) return;
    d2wr32(b + 0x2CC, d2rd32(b + 0x2CC) - 1);
    if (d2rd32(unit) == 1 && d2rd32(unit + 0x14) && (d2rd8(d2rd32(unit + 0x14) + 0x16) & 1)) d2wr32(b + 0x2C8, d2rd32(b + 0x2C8) - 1);
}
