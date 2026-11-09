// 1.13c D2Game+0xF0090 -- stamp the game context's tick and update a list node. EAX = unit, EDI = other, stdcall ret 4; the
// stored value is the caller's return address (the probe's third word). 0xA4C0 (Ordinal 10920, context lookup) runs twice and
// sees the pushed EDI as its first stack word both times. The key is the unit's +0x0C id (-1 for a null unit); when unit != other
// the context's node list (+0x38, next at +0x10) is searched for that key and the first match gets +8 = value. Then context+0x160 =
// GetTickCount (the `tick` word here).
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_f0090(uint32_t unit, uint32_t other, uint32_t value, uint32_t tick, const D2Seam* e)
{
    const uint32_t ctx = e->call(0xA4C0, 0, 0, 0, 0, 0, other);
    const uint32_t key = unit ? d2rd32(unit + 0xC) : 0xffffffffu;
    if (other != unit && ctx) {
        for (uint32_t n = d2rd32(ctx + 0x38); n; n = d2rd32(n + 0x10))
            if (d2rd32(n) == key) { d2wr32(n + 8, value); break; }
    }
    const uint32_t ctx2 = e->call(0xA4C0, 0, 0, 0, 0, 0, other);
    if (ctx2) d2wr32(ctx2 + 0x160, tick);
    return ctx2 ? tick : 0;
}
