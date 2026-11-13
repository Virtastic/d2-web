// 1.13c D2Game+0xC37F0 -- quest-event callback, ECX = quest record, EDX = event record (+0 game, +8 trigger {+4 id}, +0xC unit {+0xC id}, +0x14 argument), plain ret; the leftover EAX is mirrored per path. The unit's quest-flag buffer ent (0xA4C0, byte game+0x6D) is tested: state 0xB flag 1 set calls 0x33EB0 (mode 3, quest, argument, id; id -1 without a trigger); else flag 0xE set returns that answer; else the unit id is searched in the quest's id list (+0x1C, count ushort +0x9C) -- found calls mode 4 -- then bytes +0xC and +0x9 must be nonzero, state [q+0xE0] flag 0 must not be 1, and the module table at 0x104690 (indexed by byte +0xC < 4) gives a mode below 6 (not -1) for the call. `tb` is the address of the pinned table.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc37f0(GE& e, uint32_t q, uint32_t ctx, uint32_t tb)
{
    const uint32_t game = e.rd32(ctx), unit = e.rd32(ctx + 0xc);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t ent = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    const uint32_t p8 = e.rd32(ctx + 8);
    const uint32_t id = p8 ? e.rd32(p8 + 4) : ~0u;
    const uint32_t arg = e.rd32(ctx + 0x14);
    if (e.h(0xA568, 0, 0, 0, 0, 0, ent, 0xb, 1)) return e.h(0x33EB0, 3, q, 0, 0, 0, arg, id);
    const uint32_t r = e.h(0xA568, 0, 0, 0, 0, 0, ent, 0xb, 0xe);
    if (r) return r;
    const uint32_t cid = unit ? e.rd32(unit + 0xc) : ~0u;
    const uint32_t cnt = (uint32_t)e.rd16s(q + 0x9c) & 0xffffu;
    for (uint32_t i = 0; i < cnt; ++i)
        if (e.rd32(q + 0x1c + 4 * i) == cid) return e.h(0x33EB0, 4, q, 0, 0, 0, arg, id);
    uint32_t eax = (cnt & ~0xffu) | e.rd8(q + 0xc);
    if (!e.rd8(q + 0xc)) return eax;
    eax = (cnt & ~0xffu) | e.rd8(q + 9);
    if (!e.rd8(q + 9)) return eax;
    const uint32_t r2 = e.h(0xA568, 0, 0, 0, 0, 0, ent, e.rd32(q + 0xe0), 0);
    if (r2 == 1) return r2;
    const uint32_t m = e.rd8(q + 0xc);
    eax = (r2 & ~0xffu) | m;
    if (m >= 4) return eax;
    const uint32_t t = e.rd32(tb + 4 * m);
    if (t == ~0u || t >= 6) return t;
    return e.h(0x33EB0, t, q, 0, 0, 0, arg, id);
}
}
