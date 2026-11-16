// 1.13c D2Common+0x3AAB0: set or clear the SET flag (0x2000) of a unit's state list for a state id.
// 32-bit retail layout through raw reads (unit+0x5C pStatListEx; list+0x10 flags, bit 31 = extended).
#include "d2common_3aab0.h"

// 1.13c D2Common+0x3AAB0
extern "C" int32_t D2Common_3AAB0_StateSet(uint32_t unit, uint32_t state, uint32_t set, const D2Common3AAB0Env* e)
{
    if (!state) return 0;
    const uint32_t sle = e->rd32(unit + 0x5c);
    if (!sle) return 0;
    if (!(e->rd32(sle + 0x10) & 0x80000000u)) return 0;
    const uint32_t sl = e->lookup(sle, state);
    if (!sl) return 0;
    uint32_t fl = e->rd32(sl + 0x10);
    if (set) {
        if (fl & 0x2000u) return 0;
        e->expire(sl);
        fl = e->rd32(sl + 0x10) | 0x2000u;
    } else {
        if (!(fl & 0x2000u)) return 0;
        e->expire(sl);
        fl = e->rd32(sl + 0x10) & 0xffffdfffu;
    }
    e->wr32(sl + 0x10, fl);
    e->post(unit, sl, 1);
    return 1;
}
