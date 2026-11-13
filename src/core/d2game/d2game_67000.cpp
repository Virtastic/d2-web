// 1.13c D2Game+0x67000 -- npc 0x9A messages: 0xA3 hands the item in and counts the player, 0x92 opens the quest
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f67000(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t m18 = NE::rd16(a + 0x18), m14 = NE::rd16(a + 0x14), pl = e.rd32(a + 0xc);
    const uint32_t has = e.h(0xE660, pl, 0, 0, e.rd32(a), 0, 0x206d6468);
    const uint32_t pd = e.imp(0xA4C0, pl), buf = e.rd32(pd + 0x10 + 4 * e.rd8(e.rd32(a) + 0x6d));
    if (m14 != 0x9a) return pd;                         // the vectors skip this exit: it answers the player-data pointer
    const uint32_t data = e.rd32(q + 0x18);
    if (m18 == 0xa3) {
        const uint32_t r = e.fl(buf, 3, 0);
        if (r) return r;
        e.h(0x33880, data + 0x14, 0, 0, 0, (pl ? e.rd32(pl + 0xc) : 0xffffffffu));
        if (has) {
            e.setf(buf, 3, 0xd); e.setf(buf, 3, 1);
            e.h(0x34C00, pl, 0, 0, 0, 0, e.rd32(q + 4));
            if (e.h(0xE660, pl, 0, 0, e.rd32(a), 0, 0x206d6468)) e.h(0x365E0, 0, pl, 0, 0, 0, e.rd32(a), e.rd32(pl + 0x60));
            e.wr32(data + 0x9c, e.rd32(data + 0x9c) - 1);
            const uint32_t cls = e.h(0xDFA60, pl);
            if ((cls & 0xffff) != 0xffff) e.h(0x9C160, cls, 0, 0, 0, 0, 0xB2000000u | 0x61D40);
            if (e.rd8(q + 9)) {
                if (e.rd8(q + 0xc) == 4) {
                    e.h(0x35EB0, 5);
                    e.wr8(data + 3, 1);
                    e.h(0x33920, e.rd32(q + 4), 0, 0, 0, 0, 3, 0xd);
                    e.h(0xF8D14, 0, 0, 0, 0, 0, e.rd32(q + 0xf0));          // the code-pointer check, then the quest's own state callback with ECX = the record (not reached by the vectors)
    e.h(e.rd32(q + 0xf0) & 0xffffffu, 0, q);
                } else if (e.rd8(data + 0xa1)) {
                    const uint32_t n = e.h(0x33F30, e.rd32(q + 4), e.rd32(q + 0x10));
                    if (n) { e.h(0xF8D14, 0, 0, 0, 0, 0, e.rd32(n + 0xf0));          // the code-pointer check, then the quest's own state callback with ECX = the record (not reached by the vectors)
    e.h(e.rd32(n + 0xf0) & 0xffffffu, 0, n); }
                }
            }
        }
        return e.h(0x35910, e.rd32(a + 8), e.rd32(a + 0xc), 0, 0, 0, e.rd32(a));
    }
    if (m18 != 0x92) return m18;                        // skipped: the high word is stack residue
    e.wr8(q + 0xc, 2);
    e.say(e.rd32(q + 4), 2);
    e.wr8(data + 2, 1);
    return e.h(0x35910, e.rd32(a + 8), e.rd32(a + 0xc), 0, 0, 0, e.rd32(a));
}
}
