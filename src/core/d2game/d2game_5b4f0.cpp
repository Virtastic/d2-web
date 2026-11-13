// 1.13c D2Game+0x5B4F0 -- npc 0x1FF messages: 0x4E6D opens the quest, 0x4E7A marks the reward flags and closes it
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f5b4f0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18), m18 = NE::rd16(a + 0x18);
    if (NE::rd16(a + 0x14) != 0x1ff) return data;      // the vectors skip this exit: it answers the data pointer
    const uint32_t pl = e.rd32(a + 0xc), pd = e.imp(0xA4C0, pl), buf = e.rd32(pd + 0x10 + 4 * e.rd8(e.rd32(q + 4) + 0x6d)), fid = e.rd32(q + 0xe0);
    if (m18 == 0x4e6d) {
        e.wr8(data + 8, 1); e.wr8(q + 0xc, 2);
        e.say(e.rd32(q + 4), 2);
        e.cb(e.rd32(q + 4), 0x58890);
        return e.h(0x35910, e.rd32(a + 8), e.rd32(a + 0xc), 0, 0, 0, e.rd32(a));
    }
    if (m18 != 0x4e7a) return pd;                       // skipped likewise: the player-data pointer
    uint32_t r = e.fl(buf, fid, 1);
    if (r != 1) return r;
    e.wr8(data + 0x16, 1);
    e.imp(0xA574, buf, fid);
    e.setf(buf, fid, 5);
    if (e.fl(buf, fid, 0xd) && e.rd8(q + 0xc) != 5) {
        e.h(0x35EB0, 5);
        e.h(0xF8D14, 0, 0, 0, 0, 0, e.rd32(q + 0xf0));          // the code-pointer check, then the quest's own state callback with ECX = the record (not reached by the vectors)
    e.h(e.rd32(q + 0xf0) & 0xffffffu, 0, q);
        e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 0xd);
        e.cb(e.rd32(q + 4), 0x59620);
    }
    return e.h(0x33880, q + 0x1c, 0, 0, 0, (pl ? e.rd32(pl + 0xc) : 0xffffffffu));
}
}
