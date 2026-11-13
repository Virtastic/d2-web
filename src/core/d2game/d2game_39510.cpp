// 1.13c D2Game+0x39510: fastcall ECX = unit. Missile unit (type 3) whose row byte +4 shares a bit with byte +0x10 of the module global at 0xF8BE0: 1, else 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_39510(uint32_t unit, uint32_t dt, uint32_t gl, const D2Seam*)
{
    if (!unit || d2rd32(unit) != 3) return 0;
    const int32_t idx = (int32_t)d2rd32(unit + 4);
    if (idx < 0 || idx >= (int32_t)d2rd32(dt + 0xB6C)) return 0;
    const uint32_t row = d2rd32(dt + 0xB64) + (uint32_t)idx * 0x1A4u;
    if (!row) return 0;
    return (d2rd8(gl + 0x10) & d2rd8(row + 4)) ? 1u : 0u;
}
