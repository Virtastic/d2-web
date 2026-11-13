// 1.13c D2Game+0x23850: fastcall ECX = unit, EDX = a word passed through, one stack word (the gate; ret 4). Needs a type-1 unit with a +0x80 record and a nonzero gate.
// Sets bit 0x40 of byte +0x16 of the +0x14 record, calls 0xA472(unit, 0x24, 0x50, 0) and 0x22DA0(ECX unit, EDX word; gate). With m = min(byte rec[0x6D], 2) (0 when not above 0) and
// f = (byte rec[0x6A] != 0 || rec[0x74] != 0), the level is stat 0xC clamped to 1..(dt+0xB74 - 1); 0x21E90(EAX level) returns a row, row[m + (f+6)*3] = b is scaled:
// stats 0x36 = 0x41FB0(EAX m+0x16)*b/100, 0x37 = 0x41FB0(EAX m+0x19)*b/100, 0x38 = 0x96, all through 0xA4D2(unit, stat, value, 0).
#include "d2game_seam4e.h"
extern "C" void d2g_23850(uint32_t unit, uint32_t word, uint32_t gate, uint32_t dt, const D2Seam* e)
{
    if (!unit || d2rd32(unit) != 1) return;
    const uint32_t rec = d2rd32(unit + 0x80);
    if (!rec || !gate) return;
    const uint32_t a = d2rd32(unit + 0x14);
    if (a) d2wr8(a + 0x16, (uint8_t)(d2rd8(a + 0x16) | 0x40));
    e->call(0xA472, 0, 0, 0, 0, 0, unit, 0x24, 0x50, 0);
    e->call(0x22DA0, 0, unit, word, 0, 0, gate);
    int32_t m = (int32_t)d2rd8(rec + 0x6D);
    if (m <= 0) m = 0; else if (m > 2) m = 2;
    const uint32_t f = (d2rd8(rec + 0x6A) || d2rd32(rec + 0x74)) ? 1 : 0;
    int32_t lvl = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
    const int32_t top = (int32_t)d2rd32(dt + 0xB74) - 1;
    if (lvl <= 1) lvl = 1;
    if (lvl >= top) lvl = top;
    const uint32_t row = e->call(0x1E90,(uint32_t)lvl, 0, 0, 0, 0);
    if (!row) return;
    const int32_t b = (int32_t)d2rd32(row + (((uint32_t)m + (f + 6) * 3) * 4));
    const int32_t x = (int32_t)e->call(0x21FB0, (uint32_t)m + 0x19, 0, 0, 0, 0);
    const int32_t s37 = (int32_t)((uint32_t)x * (uint32_t)b) / 100;
    const int32_t y = (int32_t)e->call(0x21FB0, (uint32_t)m + 0x16, 0, 0, 0, 0);
    const int32_t s36 = (int32_t)((uint32_t)y * (uint32_t)b) / 100;
    e->call(0xA4D2, 0, 0, 0, 0, 0, unit, 0x36, (uint32_t)s36, 0);
    e->call(0xA4D2, 0, 0, 0, 0, 0, unit, 0x37, (uint32_t)s37, 0);
    e->call(0xA4D2, 0, 0, 0, 0, 0, unit, 0x38, 0x96, 0);
}
