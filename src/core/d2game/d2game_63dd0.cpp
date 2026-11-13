// 1.13c D2Game+0x63DD0 -- message handler of the quest 6: the four area messages, the 0x28 step and the join message
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f63dd0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t m14 = e.rd32(a + 0x14), game = e.rd32(q + 4);
    const int32_t m18 = (int32_t)e.rd32(a + 0x18);
    uint32_t eax = m14;
    if (m18 >= 0x22 && m18 <= 0x25 && e.rd8(q + 9)) {
        bool go = false;
        if (e.rd8(q + 0xc) < 3) { e.h(0x35EB0, 3); go = true; }
        if (m18 == 0x25) {
            if (e.rd8(q + 0xb) < 2) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 2); e.cb(game, 0x62700); go = true; }
        } else if (!e.rd8(q + 0xb)) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 1); go = true; }
        if (go) e.cb(game, 0x60D00);
        return 0;
    }
    if (e.rd8(q + 0xc) == 4 && m18 == 0x28) { e.wr8(q + 0xc, 5); e.say(game, 5); return 0; }
    if (eax != 1) return eax;
    if (NE::rd16(q + 0x9c)) { const uint32_t pl = e.rd32(a + 0xc); eax = e.h(0x33830, (pl ? e.rd32(pl + 0xc) : 0xffffffffu), q + 0x1c); }
    if (e.rd8(q + 0xc) != 2) return eax;
    const uint32_t buf = e.qbuf(e.rd32(a), e.rd32(a + 0xc)), fid = e.rd32(q + 0xe0);
    eax = e.fl(buf, fid, 0); if (eax == 1) return eax;
    eax = e.fl(buf, fid, 1); if (eax == 1) return eax;
    e.h(0x35EB0, 3);
    return e.cb(game, 0x60D00);
}
}
