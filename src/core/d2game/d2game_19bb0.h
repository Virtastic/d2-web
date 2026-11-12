// 1.13c D2Game+0x19BB0, +0x19A00 and +0x19AD0: three monster reaction callbacks (table entries). ecx = the game,
// edx = the monster, two stack words (ret 8): a word passed on and the attacker record (x at +4, y at +8). The
// result is a byte in al; the upper bytes of eax are whatever the last call left (the entry eax when nothing is
// called). Without a monster, a record or either coordinate nothing happens. Otherwise the common reaction runs
// (0x16970: ecx = game, edx = monster; stack: the word, the record) and its low byte is the result.
// 0x19BB0 then queues one command (0x217D0: eax = 4, ebx = 0, edi = monster; the game on the stack) and fills it:
// +0x0C x, +0x10 y, +0x14 = 0x14, +0x18 = 0xA.
// 0x19A00 and 0x19AD0 queue a command 7 first: +0x0C = 9 or 8 when the monster class (+0x04) has the matching
// record flag (0x1560: eax = class, edx = 9 or 8), else 1; +0x10 x, +0x14 y, +0x18 = 4; then the command 4 as
// above with +0x14 = 0xC.
#pragma once
#include <cstdint>

namespace g19bb0 {
template <class E> bool Guard(E& e, uint32_t mon, uint32_t rec)
{
    return mon && rec && e.rd32(rec + 4) && e.rd32(rec + 8);
}
template <class E> uint32_t Queue4(E& e, uint32_t game, uint32_t mon, uint32_t rec, uint32_t w14, uint32_t low)
{
    const uint32_t ev = e.h(0x217D0, 4, 0, 0, 0, 0, game);
    e.j(0x217D0, mon);
    if (ev) {
        e.wr32(ev + 0xC, e.rd32(rec + 4));
        e.wr32(ev + 0x10, e.rd32(rec + 8));
        e.wr32(ev + 0x14, w14);
        e.wr32(ev + 0x18, 0xA);
    }
    return (ev & 0xFFFFFF00u) | (low & 0xFF);
}
template <class E> uint32_t React(E& e, uint32_t eax0, uint32_t game, uint32_t mon, uint32_t word, uint32_t rec)
{
    if (!Guard(e, mon, rec)) return eax0 & 0xFFFFFF00u;
    const uint32_t low = e.h(0x16970, 0, game, mon, 0, 0, word, rec);
    return Queue4(e, game, mon, rec, 0x14, low);
}
template <class E> uint32_t ReactTwo(E& e, uint32_t eax0, uint32_t game, uint32_t mon, uint32_t word, uint32_t rec, uint32_t flag)
{
    if (!Guard(e, mon, rec)) return eax0 & 0xFFFFFF00u;
    const uint32_t low = e.h(0x16970, 0, game, mon, 0, 0, word, rec);
    const uint32_t ev = e.h(0x217D0, 7, 0, 0, 0, 0, game);
    e.j(0x217D0, mon);
    if (ev) {
        const uint32_t has = e.h(0x1560, e.rd32(mon + 4), 0, flag);
        e.wr32(ev + 0xC, has ? flag : 1);
        e.wr32(ev + 0x10, e.rd32(rec + 4));
        e.wr32(ev + 0x14, e.rd32(rec + 8));
        e.wr32(ev + 0x18, 4);
    }
    return Queue4(e, game, mon, rec, 0xC, low);
}
}
