// 1.13c D2Game+0x39040: missile callback, fastcall EDX = unit, stack word 1 = P (ret 8). Valid missile row: P[0x34] = P[0x30].
#include "d2game_seam4e.h"
extern "C" void d2g_39040(uint32_t unit, uint32_t p, uint32_t dt, const D2Seam*)
{
    if (!unit) return;
    const int32_t idx = (int32_t)d2rd32(unit + 4);
    if (idx < 0 || idx >= (int32_t)d2rd32(dt + 0xB6C)) return;
    const uint32_t row = d2rd32(dt + 0xB64) + (uint32_t)idx * 0x1A4u;
    if (!row) return;
    d2wr32(p + 0x34, d2rd32(p + 0x30));
}
