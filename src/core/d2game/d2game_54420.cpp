// 1.13c D2Game+0x54420: ecx = item, eax = a record (kind 6 only), stack = (a, ctx), ret 8. Returns 0 on success, 0xD on failure.
// kind = item[0x10]: 0 = gate 0xD76A0 (edx 0x219) over the item's two class fields; 1 = equip (d2ik_equip); 6 = 0xD4A70 over eax[0xC];
// otherwise 0xD. Callees go through the D2Seam. (Fatal/null-item paths are not framed: a null item faults in retail.)
#include "d2game_itemkind.h"
extern "C" uint32_t d2g_54420(uint32_t item, uint32_t eax, uint32_t a, uint32_t ctx, const D2Seam* e)
{
    const uint32_t kind = item ? d2rd32(item + 0x10) : 0;
    if (kind == 0) {
        e->call(0xA4A2, 0, 0, 0, 0, 0, item, 4);
        const uint32_t t = d2rd32(item), rec = d2rd32(item + 0x2C);
        const bool direct = (int32_t)t == 2 || ((int32_t)t > 3 && (int32_t)t <= 5);
        const uint32_t v1 = direct ? d2rd32(rec + 0x10) : rec ? e->call(0xA3B8, 0, 0, 0, 0, 0, rec) : 0;
        const uint32_t v2 = direct ? d2rd32(rec + 0xC) : rec ? e->call(0xA454, 0, 0, 0, 0, 0, rec) : 0;
        const uint32_t w[8] = { a, ctx, d2rd32(item + 0xC), v2, v1, 0, 0, 0 };
        return d2ik_gate(e, 0x219, w) ? 0 : 0xD;
    }
    if (kind == 1) return d2ik_equip(e, item, ctx, a) ? 0 : 0xD;
    if (kind == 6 && eax) {
        e->call(0xA4A2, 0, 0, 0, 0, 0, item, 4);
        uint32_t slot = a;
        const uint32_t w[8] = { a, ctx, d2rd32(eax + 0xC), (uint32_t)(uintptr_t)&slot, 0, 1, 0, 0 };
        return d2ik_d4a70(e, d2rd32(item + 0xC), a, w) ? 0 : 0xD;
    }
    return 0xD;
}
