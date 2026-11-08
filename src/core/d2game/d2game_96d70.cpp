// 1.13c D2Game+0x96D70 -- pet-list release (EAX = owner unit or 0, stack = a game word handed on in EDI, stack = the pet unit; ret 8).
// Written from the 1.13c disassembly as a rule over E. rec = 0xA4C0(unit) (null is fatal in retail), rec+0x44 = the pet table pointer (zero: return 0), whose first word is the
// array base of 0xC-byte slots; slots 1 .. count-1 (count = sgptDataTables +0xBF0) each hold a list head, nodes linked through +0x14. The node whose +4 equals the
// owner's +0xC (-1 for no owner) has bit 0 of its flags cleared, is copied into a 7-word block {+4, the pet unit's +0xC (-1 none), owner[+4] as a word (0xFFFF none), slot, +8, +0xC, 0} and
// 0xDFC10(EBX = 0x6FCB66E0 callback, EDI = the game word, &block) runs; the result is 1, else 0.
#ifndef D2GAME_RULE_96D70
#define D2GAME_RULE_96D70
#include <cstdint>

namespace g96d70 {
template <class E> uint32_t Release(E& e, uint32_t owner, uint32_t game, uint32_t unit, uint32_t g)
{
    const uint32_t rec = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t p44 = e.rd32(rec + 0x44);
    if (!p44) return 0;
    const int32_t n = (int32_t)e.rd32(e.rd32(g) + 0xBF0);
    const uint32_t key = owner ? e.rd32(owner + 0xC) : 0xFFFFFFFFu;
    for (int32_t i = 1; i < n; ++i) {
        const uint32_t slot = e.rd32(p44) + 0xCu * (uint32_t)i;
        if (!slot) continue;
        for (uint32_t nd = e.rd32(slot); nd; nd = e.rd32(nd + 0x14)) {
            if (e.rd32(nd + 4) != key) continue;
            e.wr32(nd, e.rd32(nd) & 0xFFFFFFFEu);
            uint32_t blk[7] = { e.rd32(nd + 4), unit ? e.rd32(unit + 0xC) : 0xFFFFFFFFu, (owner ? e.rd32(owner + 4) : 0xFFFFFFFFu) & 0xFFFFu, (uint32_t)i, e.rd32(nd + 8), e.rd32(nd + 0xC), 0 };
            e.h(0xDFC10, 0, 0, 0, 0xB2000000u | 0x966E0, 0, e.stk(blk));
            e.edi(0xDFC10, game);
            e.blk(0xDFC10, blk, 7);
            return 1;
        }
    }
    return 0;
}
}
#endif
