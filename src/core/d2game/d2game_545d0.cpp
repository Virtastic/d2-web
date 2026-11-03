// 1.13c D2Game+0x545D0: ecx = item, eax = a record (kind 6 only), edx = ctx, stack = (a), ret 4. Returns 0 on success, 0xC on failure.
// kind = item[0x10] (> 6 or 3 or 5: 0xC): 0 = gate 0xD76A0 (edx 0x140); 1 = 0xD60D0 (eax = item[0xC], ecx = ctx; a, class, 1, &local);
// 2 = 0xD4E80 (eax = item[0xC], ecx = ctx, edx = a; field 0x1100, 0, &local); 4 = 0xAA54 (ctx[0x60], item) then 0xD1650 (eax = its result);
// 6 = 0xD4A70 over eax[0xC]. Each arm first calls 0xA4A2(item, 4).
#include "d2game_itemkind.h"
extern "C" uint32_t d2g_545d0(uint32_t item, uint32_t eax, uint32_t ctx, uint32_t a, const D2Seam* e)
{
    const uint32_t kind = item ? d2rd32(item + 0x10) : 0;
    if (kind > 6) return 0xC;
    uint32_t slot = a;                                   // the stack slot of `a`: retail hands out its address
    uint32_t r = 0;
    switch (kind) {
    case 0: {
        e->call(0xA4A2, 0, 0, 0, 0, 0, item, 4);
        const uint32_t ic = item ? d2rd32(item + 0xC) : 0xFFFFFFFFu;
        const uint32_t r30 = d2ik_1130(e, item), r00 = d2ik_1100(e, item);
        const uint32_t w[8] = { a, ctx, ic, r00, r30, 0, 1, 0 };
        r = d2ik_gate(e, 0x140, w);
        break; }
    case 1: {
        e->call(0xA4A2, 0, 0, 0, 0, 0, item, 4);
        const uint32_t ic = item ? d2rd32(item + 0xC) : 0xFFFFFFFFu;
        const uint32_t cls = e->call(0xA97C, 0, 0, 0, 0, 0, item);
        r = e->call(0xD60D0, ic, ctx, 0, 0, 0, a, cls, 1, (uint32_t)(uintptr_t)&slot);
        break; }
    case 2: {
        e->call(0xA4A2, 0, 0, 0, 0, 0, item, 4);
        const uint32_t ic = item ? d2rd32(item + 0xC) : 0xFFFFFFFFu;
        const uint32_t r00 = d2ik_1100(e, item);
        r = e->call(0xD4E80, ic, ctx, a, 0, 0, r00, 0, (uint32_t)(uintptr_t)&slot);
        break; }
    case 4: {
        e->call(0xA4A2, 0, 0, 0, 0, 0, item, 4);
        const uint32_t x = e->call(0xAA54, 0, 0, 0, 0, 0, d2rd32(ctx + 0x60), item);
        r = e->call(0xD1650, x);
        break; }
    case 6: {
        e->call(0xA4A2, 0, 0, 0, 0, 0, item, 4);
        if (!eax) return 0xC;
        const uint32_t ec = d2rd32(eax + 0xC);
        const uint32_t w[8] = { a, ctx, ec, (uint32_t)(uintptr_t)&slot, 0, 0, 0, 0 };
        r = d2ik_d4a70(e, d2rd32(item + 0xC), ec, w);
        break; }
    default: return 0xC;                                  // kinds 3 and 5
    }
    return r ? 0 : 0xC;
}
