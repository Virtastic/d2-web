// 1.13c D2Game+0x5C9E0 -- message handler of an act 5 quest: the start message and the rune reward
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f5c9e0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18), m18 = NE::rd16(a + 0x18);
    if (NE::rd16(a + 0x14) != 0x203) return data;
    const uint32_t game = e.rd32(q + 4), pd = e.imp(0xA4C0, e.rd32(a + 0xc)), buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d)), fid = e.rd32(q + 0xe0);
    if (m18 == 0x4e80) {
        e.wr8(data + 0x84, 1); e.wr8(q + 0xc, 2);
        e.say(game, 2);
        e.cb(game, 0x58530);
        return e.h(0x35910, e.rd32(a + 8), e.rd32(a + 0xc), 0, 0, 0, e.rd32(a));
    }
    if (m18 != 0x4e8e) return pd;
    uint32_t eax = e.fl(buf, fid, 1);
    if (eax != 1) return eax;
    if (e.fl(buf, fid, 0xd)) {
        if (e.rd8(q + 0xc) != 5) {
            e.h(0x35EB0, 5);
            const uint32_t fp = e.rd32(q + 0xf0);          // the quest's own state callback, called with ECX = quest (not reached by the vectors)
            e.h(0xF8D14, 0, 0, 0, 0, 0, fp);
            e.h(fp & 0xffffffu, 0, q);
            e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 0xd);
        }
        e.wr32(q + 0xa8, 0);
    }
    uint32_t cnt = 3;
    if (e.fl(buf, 0x24, 6)) cnt = 2;
    else if (e.fl(buf, 0x24, 7)) cnt = 1;
    static const uint32_t rune[3] = { 0x20373072, 0x20383072, 0x20393072 };
    bool got = false;
    for (uint32_t i = 0; i < cnt; i++) if (e.h(0x36470, 0, rune[i], e.rd32(a + 0xc), 0, 0, e.rd32(a), 2, 1)) got = true;
    if (got) {
        e.setf(buf, fid, 0); e.clrf(buf, fid, 1); e.imp(0xA574, buf, fid);
        uint8_t pkt[8] = { 0x5d, 0x20, 2, 0, 0, 0, 0, 0 };
    const uint32_t pl = e.rd32(a + 0xc);
    const uint32_t cli = pl && !e.rd32(pl) ? e.rd32(e.imp(0xA4C0, pl) + 0x9c) : 0;
        e.h(0x33880, q + 0x1c, 0, 0, 0, pl ? e.rd32(pl + 0xc) : 0xffffffffu);
        e.h(0x8A3E0, cli, 0, 0, 0, 0, e.stk(pkt), 6);
    }
    return e.h(0x35910, e.rd32(a + 8), e.rd32(a + 0xc), 0, 0, 0, e.rd32(a));
}
}
