// 1.13c D2Game+0x5F200 -- message handler of the quest 0x27: start, the two state steps and the five per-npc flags
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f5f200(NE& e, uint32_t eax0, uint32_t q, uint32_t a)
{
    const uint32_t m18 = NE::rd16(a + 0x18), m14 = NE::rd16(a + 0x14), data = e.rd32(q + 0x18), game = e.rd32(q + 4);
    uint32_t eax = (eax0 & 0xffff0000u) | m18;
    if (m18 == 0x4eb9 && m14 == 0x203) {
        eax = NE::lo(eax, e.rd8(q + 9));
        if (!e.rd8(q + 9)) return eax;
        eax = NE::lo(eax, e.rd8(q + 0xc));
        if (e.rd8(q + 0xc) != 1) return eax;
        e.h(0x35EB0, 2);
        if (e.rd8(q + 0xb) != 1) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 1); e.cb(game, 0x58F90); }
        eax = e.cb(game, 0x57750);
        e.wr8(data + 1, 1);
        return eax;
    }
    if (m18 == 0x4e22) {
        if (e.rd8(q + 9) && e.rd8(q + 0xc) < 4 && e.rd8(q + 0xb) != 3) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 3); e.cb(game, 0x58F90); }
        eax = e.rd32(data + 0xc);
        if ((int32_t)eax > 0) return eax;
        for (uint32_t i = 0; i < 3; i++) if (e.rd32(data + 0x30 + 4 * i)) return i;
        eax = e.h(0x5E560, 0, 0, 0, 0, 0, q, data);
        if (!eax) return eax;
        e.wr8(data + 0x10, 1); e.wr8(data + 0x11, 1);
        return NE::lo(eax, 1);
    }
    if (m18 != 0x4eb9 && m18 == 0x4ec9) return e.setf(e.qbuf(game, e.rd32(a + 0xc)), 0x27, 4);
    uint32_t want, f;
    switch (m14) {
    case 0x1ff: want = 0x4ec7; f = 5; break;
    case 0x200: want = 0x4ec6; f = 7; break;
    case 0x201: want = 0x4ec8; f = 8; break;
    case 0x203: want = 0x4ec4; f = 9; break;
    case 0x208: want = 0x4ec5; f = 6; break;
    default: return eax;
    }
    if (m18 != want) return eax;
    return e.setf(e.h(0x1020, e.rd32(a + 0xc), 0, 0, 0, 0, game), e.rd32(q + 0xe0), f);
}
}
