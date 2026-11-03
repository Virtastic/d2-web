// D2Game+0xEF6B0 -- picks a monster class from a level-record's class list and asks 0xEF650 about it: four stack args (ret 0x10), arg1 = unit. Written from the 1.13c disassembly; callees are the D2Seam.
// rec = 0xA74E(0xA556(unit)): byte +0x33 = class count n (0 is fatal 0x4F0), shorts from +0x36. The roll 0x11D0 (ECX = unit+0x6c or 0, ESI = n) is the start; each step advances
// (wrapping at n), reads the class, takes its monstats record (sgptDataTables +0xA78 base, +0xA80 count, 0x1A8 each; none for an out-of-range class) and stops when
// byte +0xC & the low byte of gdwBitMasks[0] is set. Giving up after n steps is fatal 0x507. Result: 0xEF650(ECX = class, EDX = 1, stack a0, unit, a2, a3, 5, 0) != 0.
// dtSlot / maskSlot are the values of the module slots 0xF829C and 0xF8BE0 (pointer to the datatables pointer; pointer to the mask array).
#include "d2game_seam4e.h"
// 1.13c D2Game+0xEF6B0
extern "C" uint32_t d2game_ef6b0(uint32_t a0, uint32_t unit, uint32_t a2, uint32_t a3, uint32_t dtSlot, uint32_t maskSlot, const D2Seam* e)
{
    auto fatal = [&](uint32_t code) {
        const uint32_t s = e->call(0xA2DA, 0, 0, 0, 0, 0, code);
        e->call(0xA2C2, 0, 0, 0, 0, 0, 0, s);
        e->call(0x2A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
    };
    const uint32_t rec = e->call(0xA74E, 0, 0, 0, 0, 0, e->call(0xA556, 0, 0, 0, 0, 0, unit));
    const uint32_t n = d2rd8(rec + 0x33);
    if (!n) fatal(0x4F0);
    uint32_t edi = e->call(0x11D0, 0, unit ? unit + 0x6C : 0, 0, 0, n);
    const uint32_t mask = d2rd32(d2rd32(maskSlot)) & 0xFF;
    int32_t cx = 0;
    uint32_t cnt = 0;
    for (;;) {
        edi = (edi + 1 >= n) ? 0 : edi + 1;
        cx = (int16_t)d2rd16(rec + edi * 2 + 0x36);
        uint32_t r = 0;
        if (cx >= 0) {
            const uint32_t dt = d2rd32(d2rd32(dtSlot));
            if (cx < (int32_t)d2rd32(dt + 0xA80)) r = d2rd32(dt + 0xA78) + (uint32_t)cx * 0x1A8;
        }
        if (d2rd8(r + 0xC) & mask) break;
        if (++cnt >= n) break;
    }
    if (cx == -1) return 0;
    if (cnt >= n) { fatal(0x507); return 0; }
    return e->call(0xEF650, 0, (uint32_t)cx, 1, 0, 0, a0, unit, a2, a3, 5, 0) != 0;
}
