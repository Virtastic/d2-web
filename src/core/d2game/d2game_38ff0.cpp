// 1.13c D2Game+0x38FF0: missile callback, fastcall (ECX unused, EDX = unit whose +4 is a row index into the missile table, stack word 0 = target,
// word 1 unused, ret 8). dt = *sgptDataTables (the word at 0x6FD1829C); rows start at dt[0xB64], 0x1A4 bytes each, count dt[0xB6C]. A null unit, an index
// outside [0, count), a null row or a null target ends the function; a row whose word at +0x36 is above zero (signed 16-bit) calls 0xA83E (stdcall:
// target, the word sign-extended, 0). Void: the probe records no result.
#include "d2game_seam4e.h"
extern "C" void d2g_38ff0(uint32_t unit, uint32_t target, uint32_t dt, const D2Seam* e)
{
    if (!unit) return;
    const int32_t idx = (int32_t)d2rd32(unit + 4);
    if (idx < 0 || idx >= (int32_t)d2rd32(dt + 0xB6C)) return;
    const uint32_t row = d2rd32(dt + 0xB64) + (uint32_t)idx * 0x1A4u;
    if (!row || !target) return;
    const int16_t w = (int16_t)d2rd16(row + 0x36);
    if (w <= 0) return;
    e->call(0xA83E, 0, 0, 0, 0, 0, target, (uint32_t)(int32_t)w, 0);
}
