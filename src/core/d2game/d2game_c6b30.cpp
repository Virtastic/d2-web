// 1.13c D2Game+0xC6B30 -- quest-event callback, ECX = quest record (+4 game, +9 active, +0xB/+0x14 stage bytes, +0xC state, +0x1C id list, +0xE0 flag state, +0xF0 code pointer), EDX = event record (+0 game, +8 trigger, +0xC unit, +0x14 argument, word +0x18 event id); plain ret (EAX mirrored). Event 0x15C with an active record and argument 0xB1 resets stage +0xB 1 to 2 and, when state +0xC is 1, runs 0x35EB0 (EAX = 2, ESI = record) and posts callback 0xC3750 through 0xDFCC0 (EAX 0, EDI = game, stack 0). Events 0x16A..0x174: an active record in stage not-5 whose flag (state +0xE0 / 0xD) is set gets +0x14 = 0, +0xB = 0xD, 0x35EB0 (EAX = 5) and its code pointer called (ECX = record; IsBadCodePtr first, bad is the fatal path 0xDD, not vectored); then 0x35910 (EAX = trigger, ECX = unit, stack = game), and when flag +0xE0 / 1 is set: flags 0xB/0 cleared and 0xB/1 set, 0x33880 (EAX = record + 0x1C, ESI = unit id or -1) and, for an inactive record, 0x33920 (EAX = record game, stack 0xB, 0xD). Any other id returns itself.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
static uint32_t slotOf6b30(GE& e, uint32_t p)
{
    for (int k = 2; k < e.nb; k++) { const uint32_t b = e.B(k); if (p >= b && p < b + fnBlobLen(e.r, k)) return 0xF00000u + (uint32_t)k * 0x10000u + (p - b); }
    return 0;
}
uint32_t fc6b30(GE& e, uint32_t q, uint32_t ev)
{
    const uint32_t id = (uint32_t)e.rd16s(ev + 0x18) & 0xffffu;
    if (id == 0x15c) {
        const uint32_t eax = (id & ~0xffu) | e.rd8(q + 9);
        if (!e.rd8(q + 9) || e.rd32(ev + 0x14) != 0xb1) return eax;
        if (e.rd8(q + 0xb) == 1) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 2); }
        if (e.rd8(q + 0xc) != 1) return 2;
        e.h(0x35EB0, 2, 0, 0, 0, q);
        const uint32_t game = e.rd32(q + 4);
        const uint32_t v = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC3750, 0, 0); e.edi(0xDFCC0, game);
        return v;
    }
    if (id <= 0x169 || id > 0x174) return id;
    const uint32_t unit = e.rd32(ev + 0xc), game = e.rd32(ev);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t ent = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (e.rd8(q + 0xc) != 5 && e.rd8(q + 9) == 1 && e.h(0xA568, 0, 0, 0, 0, 0, ent, e.rd32(q + 0xe0), 0xd)) {
        e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 0xd);
        e.h(0x35EB0, 5, 0, 0, 0, q);
        e.h(slotOf6b30(e, q + 0xf0), 0, q);
    }
    uint32_t eax = e.h(0x35910, e.rd32(ev + 8), e.rd32(ev + 0xc), 0, 0, 0, e.rd32(ev));
    if (!e.h(0xA568, 0, 0, 0, 0, 0, ent, e.rd32(q + 0xe0), 1)) return 0;
    e.h(0xA57A, 0, 0, 0, 0, 0, ent, 0xb, 0);
    e.h(0xA586, 0, 0, 0, 0, 0, ent, 0xb, 1);
    const uint32_t u = e.rd32(ev + 0xc);
    eax = e.h(0x33880, q + 0x1c, 0, 0, 0, u ? e.rd32(u + 0xc) : ~0u);
    eax = (eax & ~0xffu) | e.rd8(q + 9);
    if (e.rd8(q + 9)) return eax;
    return e.h(0x33920, e.rd32(q + 4), 0, 0, 0, 0, 0xb, 0xd);
}
}
