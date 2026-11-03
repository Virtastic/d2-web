// 1.13c D2Game+0x967B0: eax = unit, esi = idx (plain ret). q = [0xA4C0(unit) +0x44]; idx in [0, dt+0xBF0) picks the 12-byte row [q]+12*idx; the answer is [[row] +4], else -1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_967b0(uint32_t unit, uint32_t idx, uint32_t dt, const D2Seam* e)
{
    const uint32_t r = e->call(0xA4C0, 0, 0, 0, 0, 0, unit);
    if (!r) return 0xFFFFFFFFu;
    const uint32_t q = d2rd32(r + 0x44);
    if (!q || (int32_t)idx < 0 || (int32_t)idx >= (int32_t)d2rd32(dt + 0xBF0)) return 0xFFFFFFFFu;
    const uint32_t row = d2rd32(q) + idx * 12;
    if (!row) return 0xFFFFFFFFu;
    const uint32_t v = d2rd32(row);
    if (!v) return 0xFFFFFFFFu;
    return d2rd32(v + 4);
}
