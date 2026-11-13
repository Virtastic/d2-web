// 1.13c D2Game+0x65260 -- npc 0x94 message: 0x40 moves quest to state 2; 0x4C closes the reward and marks the player flags
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f65260(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18);
    if (NE::rd16(a + 0x14) != 0x94) return data;
    const uint32_t m18 = NE::rd16(a + 0x18), pl = e.rd32(a + 0xc), pd = e.imp(0xA4C0, pl), buf = e.rd32(pd + 0x10 + 4 * e.rd8(e.rd32(q + 4) + 0x6d)), fid = e.rd32(q + 0xe0);
    if (m18 == 0x40) {
        e.wr8(data + 0x86, 1); e.wr8(q + 0xc, 2);
        e.say(e.rd32(q + 4), 2);
        e.cb(e.rd32(q + 4), 0x62370);
        return e.h(0x35910, e.rd32(a + 8), pl, 0, 0, 0, e.rd32(a));
    }
    if (m18 != 0x4c) return pd;
    uint32_t r = e.fl(buf, fid, 1);
    if (r != 1) return r;
    if (e.fl(buf, fid, 0xd)) {
        if (e.rd8(q + 0xc) != 5) {
            e.h(0x35EB0, 5);
            e.h(0xF8D14, 0, 0, 0, 0, 0, e.rd32(q + 0xf0));     // the quest's own state callback (not reached by the vectors)
            e.h(e.rd32(q + 0xf0) & 0xffffffu, 0, q);
            e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 0xd);
        }
        e.wr32(q + 0xa8, 0);
    }
    e.setf(buf, fid, 0); e.clrf(buf, fid, 1);
    e.setf(buf, e.rd32(q + 0xe4), 0xd); e.setf(buf, e.rd32(q + 0xe4), 1);
    e.imp(0xA574, buf, fid);
    e.imp(0xA4D2, pl, 5, 1, 0);
    e.h(0x33880, q + 0x1c, 0, 0, 0, (pl ? e.rd32(pl + 0xc) : 0xffffffffu));
    return e.h(0x35910, e.rd32(a + 8), pl, 0, 0, 0, e.rd32(a));
}
}
