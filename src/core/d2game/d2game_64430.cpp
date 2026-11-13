// 1.13c D2Game+0x64430 -- message handler of the quest 5: the 0x7F step and the six npc messages that move a player between the two lists
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f64430(NE& e, uint32_t eax0, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18), game = e.rd32(q + 4), m14 = NE::rd16(a + 0x14), m18 = NE::rd16(a + 0x18);
    uint32_t eax = eax0;
    if (m18 == 0x7f) {
        eax = NE::lo(eax, e.rd8(q + 9));
        if (e.rd8(q + 9)) {
            bool flag = false;
            if (e.rd8(data + 0x11b) == 1) {
                if (e.rd8(q + 0xb) < 1) { flag = true; e.wr8(q + 0xb, 1); e.wr8(q + 0x14, 0); e.cb(game, 0x62870); }
                if (e.rd8(q + 0xb) == 3) {
                    flag = true; e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 2);
                    e.cb(game, 0x62870);
                    if (e.rd8(q + 0xc) < 3) e.h(0x35EB0, 3);
                }
            }
            eax = 2;
            bool go = flag;
            if (e.rd8(q + 0xc) < 2) { e.h(0x35EB0, 2); go = true; }
            else eax = NE::lo(eax, flag ? 1 : 0);
            if (go) eax = e.cb(game, 0x61250);
        }
    }
    if (m14 != 0x9a && m14 != 0x96 && m14 != 0x109 && m14 != 0x9b && m14 != 0x94 && m14 != 0x93) return eax;
    if (m18 < 0x8c || m18 > 0x91) return eax;
    const uint32_t pl = e.rd32(a + 0xc);
    eax = e.fl(e.qbuf(e.rd32(a), pl), 5, 0xd);
    if (eax && e.rd8(data + 0x11a)) {
        e.wr8(data + 0x11a, 0); e.wr8(q + 0xc, 5);
        e.say(game, 5);
        e.h(0xF8D14, 0, 0, 0, 0, 0, e.rd32(q + 0xf0));          // the code-pointer check, then the quest's own state callback (not reached by the vectors)
        e.h(e.rd32(q + 0xf0) & 0xffffffu, 0, q);
    }
    const uint32_t uid = (pl ? e.rd32(pl + 0xc) : 0xffffffffu), n = NE::rd16(data + 0x64);
    if (!n) return 0;
    uint32_t i = 0;
    while (i < n && e.rd32(data + 0x34 + 4 * i) != uid) i++;
    if (i == n) return n;
    if (i != n - 1) e.wr32(data + 0x34 + 4 * i, e.rd32(data + 0x30 + 4 * n));
    const uint32_t w = NE::rd16(data + 0x30);
    NE::wr16(data + 0x64, n - 1);
    if (w >= 0xc) return w;
    e.wr32(data + 4 * w, uid);
    NE::wr16(data + 0x30, w + 1);
    return w;
}
}
