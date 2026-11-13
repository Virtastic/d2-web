// 1.13c D2Game+0xC4EA0 -- quest-event callback, ECX = quest record, EDX = event record (+0 game, +8 trigger {+4 id}, +0xC unit {+0xC id}, +0x14 argument), plain ret; the leftover EAX is mirrored per path. Trigger id 0x14B: the game's list holder (q+4 game, +0x10F4) must exist (else the fatal path), then byte [q+0x18]+0xE == 0 calls 0x33EB0 with mode 7, otherwise the holder's 64-bit seed (+0x18 lo, +0x1C hi) is stepped (lo*0x6AC690C5 + hi) and mode = 8 + (new lo mod 3). Otherwise (id -1 without a trigger): state 0xC flag 1 set calls mode 4; else the unit id is searched in the quest's id list (+0x1C, count ushort +0x9C), found calls mode 5; else bytes +0xC and +9 must be nonzero, state 0xC flag 0 must not be 1, byte +0xC >= 5 needs state 0xC flag 0xD set, and the module table at 0x104FD8 (indexed by byte +0xC) gives a mode below 0xC (not -1). `tb` is the address of the pinned table.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc4ea0(GE& e, uint32_t q, uint32_t ctx, uint32_t tb)
{
    const uint32_t game = e.rd32(ctx), unit = e.rd32(ctx + 0xc);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t ent = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    const uint32_t p8 = e.rd32(ctx + 8);
    const uint32_t id = p8 ? e.rd32(p8 + 4) : ~0u;
    const uint32_t arg = e.rd32(ctx + 0x14);
    if (p8 && id == 0x14b) {
        const uint32_t holder = e.rd32(e.rd32(q + 4) + 0x10f4);
        if (!holder) {
            const uint32_t msg = e.h(0xA2DA, 0, 0, 0, 0, 0, 0x1fb);
            e.h(0xA2C2, 0, 0, 0, 0, 0, 0x6fd19ff9, msg);
            e.h(0x2A3C, 0, 0, 0, 0, 0, 0xffffffffu);
            return 0;
        }
        if (!e.rd8(e.rd32(q + 0x18) + 0xe)) return e.h(0x33EB0, 7, q, 0, 0, 0, arg, 0x14b);
        const uint64_t t = (uint64_t)e.rd32(holder + 0x18) * 0x6ac690c5u + e.rd32(holder + 0x1c);
        e.wr32(holder + 0x1c, (uint32_t)(t >> 32));
        e.wr32(holder + 0x18, (uint32_t)t);
        return e.h(0x33EB0, (uint32_t)t % 3 + 8, q, 0, 0, 0, arg, 0x14b);
    }
    if (e.h(0xA568, 0, 0, 0, 0, 0, ent, 0xc, 1)) return e.h(0x33EB0, 4, q, 0, 0, 0, arg, id);
    const uint32_t cid = unit ? e.rd32(unit + 0xc) : ~0u;
    const uint32_t cnt = (uint32_t)e.rd16s(q + 0x9c) & 0xffffu;
    for (uint32_t i = 0; i < cnt; ++i)
        if (e.rd32(q + 0x1c + 4 * i) == cid) return e.h(0x33EB0, 5, q, 0, 0, 0, arg, id);
    uint32_t eax = (cnt & ~0xffu) | e.rd8(q + 0xc);
    if (!e.rd8(q + 0xc)) return eax;
    eax = (cnt & ~0xffu) | e.rd8(q + 9);
    if (!e.rd8(q + 9)) return eax;
    const uint32_t r = e.h(0xA568, 0, 0, 0, 0, 0, ent, 0xc, 0);
    if (r == 1) return r;
    if (e.rd8(q + 0xc) >= 5 && !e.h(0xA568, 0, 0, 0, 0, 0, ent, 0xc, 0xd)) return 0;
    const uint32_t t = e.rd32(tb + 4 * e.rd8(q + 0xc));
    if (t == ~0u || t >= 0xc) return t;
    return e.h(0x33EB0, t, q, 0, 0, 0, arg, id);
}
}
