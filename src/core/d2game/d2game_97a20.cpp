// 1.13c D2Game+0x97A20 -- append a pet-list entry (ESI = list {head, count, max}, EBX = game, stack = a refill word handed to 0x96FA0, stack = the pet unit or 0,
// stack = a pointer to three words; ret 12). Written from the 1.13c disassembly as a rule over E.
// A zero max asks 0x96FA0(game, word) to fill the list; still zero: 0x977E0(EAX = 0, ECX = game, EDX = the unit's +0xC or -1, EBX = game) and return 0.
// A full list (count == max) drops its head entry through 0x97920(EAX = game, ECX = the head's id, EBX = game, list, 1) and a list still full, or an empty head,
// is fatal. The entry (0xA2E6: ECX = game+0x1C pool, EDX = 0x18; file, 0x322, 0) is zeroed, +4 = the unit's +0xC (-1 for none), +8/+0xC/+0x10 = the three words, linked
// after the last node (or as the head); count + 1 above max is fatal; the result is 1.
#ifndef D2GAME_RULE_97A20
#define D2GAME_RULE_97A20
#include <cstdint>
#include "d2game_97920.cpp"

namespace g97a20 {
template <class E> uint32_t Add(E& e, uint32_t lst, uint32_t game, uint32_t a0, uint32_t unit, uint32_t a2)
{
    uint32_t mx = e.rd32(lst + 8);
    if (!mx) {
        e.h(0x96FA0, 0, 0, 0, 0, 0, game, a0);
        mx = e.rd32(lst + 8);
        if (!mx) { e.h(0x977E0, 0, game, 0, game); return 0; }   // EDX (the unit id) is not among the hook's logged registers
    }
    if (e.rd32(lst + 4) == mx) {
        const uint32_t head = e.rd32(lst);
        if (!head) return 0;   // fatal in retail
        g97920::Remove(e, game, e.rd32(head + 4), game, lst, 1);
        if (e.rd32(lst + 4) == e.rd32(lst + 8)) return 0;   // fatal in retail
    }
    uint32_t tail = e.rd32(lst);
    if (tail) while (e.rd32(tail + 0x14)) tail = e.rd32(tail + 0x14);
    const uint32_t nd = e.h(0xA2E6, 0, e.rd32(game + 0x1C), 0x18, 0, 0, 0xB2000000u | 0xFBA38, 0x322, 0);
    for (uint32_t o = 0; o < 0x18; o += 4) e.wr32(nd + o, 0);
    e.wr32(nd + 4, unit ? e.rd32(unit + 0xC) : 0xFFFFFFFFu);
    e.wr32(nd + 8, e.rd32(a2)); e.wr32(nd + 0xC, e.rd32(a2 + 4)); e.wr32(nd + 0x10, e.rd32(a2 + 8));
    if (tail) e.wr32(tail + 0x14, nd); else e.wr32(lst, nd);
    const uint32_t c = e.rd32(lst + 4) + 1;
    e.wr32(lst + 4, c);
    if ((int32_t)c > (int32_t)e.rd32(lst + 8)) return 0;   // fatal in retail
    return 1;
}
}
#endif
