// 1.13c D2Game+0x39080: missile callback, fastcall EDX = unit, stack word 1 = P (ret 8). A valid missile row (dt+0xB64, 0x1A4 each) gives c = max(row[0x78], 0); s = stat 0x38 of the unit
// (0xA42A(unit, 0x38, 0)). P[0x34] = s*c/100 and P[0x30] = 0. The product is divided first when one side is above 0x100000 / 0x10000 and its low nibble-cleared value is at least 0x640
// (so the 32-bit multiply cannot overflow); otherwise the full 64-bit product is divided by 100 (signed) and its low word kept.
#include "d2game_seam4e.h"
extern "C" void d2g_39080(uint32_t unit, uint32_t p, uint32_t dt, const D2Seam* e)
{
    if (!unit) return;
    const int32_t idx = (int32_t)d2rd32(unit + 4);
    if (idx < 0 || idx >= (int32_t)d2rd32(dt + 0xB6C)) return;
    const uint32_t row = d2rd32(dt + 0xB64) + (uint32_t)idx * 0x1A4u;
    if (!row) return;
    const int32_t s = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0x38, 0);
    int32_t c = (int32_t)d2rd32(row + 0x78);
    if (c < 0) c = 0;
    int32_t r;
    const int64_t full = (int64_t)c * s;
    if (s > 0x100000) {
        if ((s & ~0xF) < 0x640) r = (int32_t)(full / 100);
        else r = (s / 100) * c;
    } else if (c > 0x10000) {
        if ((c & ~0xF) >= 0x640) r = (c / 100) * s;
        else r = (int32_t)(full / 100);
    } else {
        r = (int32_t)((uint32_t)c * (uint32_t)s) / 100;
    }
    d2wr32(p + 0x34, (uint32_t)r);
    d2wr32(p + 0x30, 0);
}
