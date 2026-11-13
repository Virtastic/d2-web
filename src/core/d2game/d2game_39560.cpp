// 1.13c D2Game+0x39560: missile callback, fastcall EDX = unit, stack word 1 = P (ret 8). Valid missile row: v = 0xB26A(unit); ecx = row[0x78] if > 0, else the skill record's +0x14C (0 if v is out of range or the record is null); P[0x44] = ecx, P[0x60] = 0x60. A null row or bad index writes nothing.
#include "d2game_seam4e.h"
extern "C" void d2g_39560(uint32_t unit, uint32_t p, uint32_t dt, const D2Seam* e)
{
    if (!unit) return;
    const int32_t idx = (int32_t)d2rd32(unit + 4);
    if (idx < 0 || idx >= (int32_t)d2rd32(dt + 0xB6C)) return;
    const uint32_t row = d2rd32(dt + 0xB64) + (uint32_t)idx * 0x1A4u;
    if (!row) return;
    const int32_t v = (int32_t)e->call(0xB26A, 0, 0, 0, 0, 0, unit);
    int32_t c = (int32_t)d2rd32(row + 0x78);
    if (c <= 0) {
        uint32_t rec = 0;
        if (v >= 0 && v < (int32_t)d2rd32(dt + 0xBA0)) rec = d2rd32(dt + 0xB98) + (uint32_t)v * 0x23Cu;
        c = rec ? (int32_t)d2rd32(rec + 0x14C) : 0;
    }
    d2wr32(p + 0x44, (uint32_t)c);
    d2wr32(p + 0x60, 0x60);
}
