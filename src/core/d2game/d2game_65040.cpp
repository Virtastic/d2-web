// 1.13c D2Game+0x65040 -- message handler of the quest 1: the message 8 step and the join message
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f65040(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t m14 = e.rd32(a + 0x14), data = e.rd32(q + 0x18), game = e.rd32(q + 4);
    uint32_t eax = m14;
    if (e.rd32(a + 0x18) == 8) {
        eax = NE::lo(eax, e.rd8(q + 9));
        if (!e.rd8(q + 9)) return eax;
        const uint32_t c = e.rd8(q + 0xc);
        eax = NE::lo(eax, c);
        bool moved = false;
        if (c == 1 || c == 2) { e.wr8(q + 0xc, 3); e.say(game, 3); eax = 0; moved = true; }
        e.wr8(data + 0x85, 1);
        if (e.rd8(q + 0xb) < 2) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 2); e.cb(game, 0x62D30); e.wr32(q + 0xa8, 0); }
        else if (!moved) return eax;
        return e.cb(game, 0x62370);
    }
    if (m14 != 1) return eax;
    if (NE::rd16(q + 0x9c)) { const uint32_t pl = e.rd32(a + 0xc); eax = e.h(0x33830, (pl ? e.rd32(pl + 0xc) : 0xffffffffu), q + 0x1c); }
    if (e.rd8(q + 0xc) != 2) return eax;
    const uint32_t buf = e.qbuf(e.rd32(a), e.rd32(a + 0xc)), fid = e.rd32(q + 0xe0);
    eax = e.fl(buf, fid, 0); if (eax == 1) return eax;
    eax = e.fl(buf, fid, 1); if (eax == 1) return eax;
    e.h(0x35EB0, 3);
    eax = NE::lo(e.cb(game, 0x62370), 1);
    if (e.rd8(q + 0xb) == 1) return eax;
    e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 1);
    eax = e.cb(game, 0x62D30);
    e.wr32(q + 0xa8, 0);
    return eax;
}
}
