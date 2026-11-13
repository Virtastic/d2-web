// 1.13c D2Game+0xC67C0 -- quest-event callback, ECX = quest record (+4 game, +0xB/+0x14 stage bytes, +0xC state, +0x1C id list), EDX = event record (+0 game, +8 trigger, +0xC unit, word +0x18 event id); plain ret (EAX = the 0x35910 answer, or the event id when the event is not one of the eleven 0x1A3..0x1AD ids other than 0x1A2/0x1AE-excluded). A matching event whose unit has quest-flag 0xD/1 set: with flag 0xD/0xD also set it clears +0x14, sets +0xB = 0xD, posts callback 0xC4E30 through 0xDFCC0 (EAX 0, EDI = game, stack 0), runs 0x33920 (EAX = game, stack 0xD, 0xD), sets +0xC = 3 and, with the module debug flag (global 0x111C20), tells 0xCBE70 the formatted state; then (flag 0xD/1 only) 0x33880 (EAX = record + 0x1C, ESI = unit id or -1) and clears flag 0xD/0 / sets flag 0xD/1 in the quest-flag buffer. Every matching or not matching path ends in 0x35910 (EAX = trigger, ECX = unit, stack = game) except an unmatched id, which returns at once. `dbg` is the address of the pinned flag.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc67c0(GE& e, uint32_t q, uint32_t ev, uint32_t dbg)
{
    const uint32_t id = (uint32_t)e.rd16s(ev + 0x18) & 0xffffu;
    switch (id) {
    case 0x1ab: case 0x1a8: case 0x1a3: case 0x1a7: case 0x1a6: case 0x1aa:
    case 0x1ad: case 0x1a9: case 0x1a5: case 0x1a4: case 0x1ac: break;
    default: return id;
    }
    const uint32_t unit = e.rd32(ev + 0xc), game = e.rd32(ev);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t ent = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (e.h(0xA568, 0, 0, 0, 0, 0, ent, 0xd, 1)) {
        if (e.h(0xA568, 0, 0, 0, 0, 0, ent, 0xd, 0xd)) {
            const uint32_t qg = e.rd32(q + 4);
            e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 0xd);
            e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC4E30, 0, 0); e.edi(0xDFCC0, qg);
            e.h(0x33920, qg, 0, 0, 0, 0, 0xd, 0xd);
            e.wr8(q + 0xc, 3);
            char buf[0x80];   // wsprintfA(buf, "%d", 3): text only seen by 0xCBE70, recorded by address
            if (e.rd32(dbg)) e.h(0xCBE70, 0, 0, (uint32_t)(uintptr_t)buf, 0, 0, qg, 4);
        }
        const uint32_t u = e.rd32(ev + 0xc);
        e.h(0x33880, q + 0x1c, 0, 0, 0, u ? e.rd32(u + 0xc) : ~0u);
        e.h(0xA57A, 0, 0, 0, 0, 0, ent, 0xd, 0);
        e.h(0xA586, 0, 0, 0, 0, 0, ent, 0xd, 1);
    }
    return e.h(0x35910, e.rd32(ev + 8), e.rd32(ev + 0xc), 0, 0, 0, e.rd32(ev));
}
}
