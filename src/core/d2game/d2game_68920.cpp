// 1.13c D2Game+0x68920 -- npc 0x96 messages: 0x51 opens the quest, 0x5C closes the reward
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f68920(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t m18 = NE::rd16(a + 0x18), pl = e.rd32(a + 0xc);
    const uint32_t pd = e.imp(0xA4C0, pl), buf = e.rd32(pd + 0x10 + 4 * e.rd8(e.rd32(a) + 0x6d)), fid = e.rd32(q + 0xe0);
    if (NE::rd16(a + 0x14) != 0x96) return pd;          // the vectors skip this exit: it answers the player-data pointer
    if (m18 == 0x51) {
        e.wr8(e.rd32(q + 0x18) + 3, 1); e.wr8(q + 0xc, 2);
        e.say(e.rd32(q + 4), 2);
        e.cb(e.rd32(q + 4), 0x62040);
        return e.h(0x35910, e.rd32(a + 8), e.rd32(a + 0xc), 0, 0, 0, e.rd32(a));
    }
    if (m18 != 0x5c) return m18;                        // skipped: the high word is stack residue
    if (!e.fl(buf, fid, 1)) return 0;
    if (e.fl(buf, fid, 0xd) && e.rd8(q + 0xc) != 5) {
        e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 0xd);
        e.h(0x35EB0, 5);
        e.h(0xF8D14, 0, 0, 0, 0, 0, e.rd32(q + 0xf0));          // the code-pointer check, then the quest's own state callback with ECX = the record (not reached by the vectors)
    e.h(e.rd32(q + 0xf0) & 0xffffffu, 0, q);
    }
    e.setf(buf, fid, 0); e.clrf(buf, fid, 1);
    e.h(0x34C00, pl, 0, 0, 0, 0, e.rd32(q + 4));
    e.h(0x33880, q + 0x1c, 0, 0, 0, (pl ? e.rd32(pl + 0xc) : 0xffffffffu));
    e.h(0xC1030, 0, 0, 0, 0, 0, e.rd32(q + 4), pl, 0x96);
    return e.h(0x35910, e.rd32(a + 8), e.rd32(a + 0xc), 0, 0, 0, e.rd32(a));
}
}
