// 1.13c D2Game+0xBB250 -- npc messages of quest 0x1A part two: 0x16F/0x2A9 opens it, 0x2AC and 0x2AD clear a flag, 0x4E20 and 0x4E21 set one
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fbb250(NE& e, uint32_t eax0, uint32_t q, uint32_t a)
{
    const uint32_t m14 = NE::rd16(a + 0x14), m18 = NE::rd16(a + 0x18), data = e.rd32(q + 0x18), other = (eax0 & 0xffff0000u) | m14;
    if (m14 == 0x16f) {
        if (m18 == 0x2a9) {
            e.wr8(data, 1); e.wr8(q + 0xc, 2);
            e.say(e.rd32(q + 4), 2);
            e.cb(e.rd32(q + 4), 0xB9290);
            return e.h(0x35910, e.rd32(a + 8), e.rd32(a + 0xc), 0, 0, 0, e.rd32(a));
        }
        if (m18 == 0x2ac) return e.clrf(e.qbuf(e.rd32(q + 4), e.rd32(a + 0xc)), 0x1a, 7);
        if (m18 != 0x4e20) return other;
        const uint32_t game = e.rd32(q + 4);
        if (!e.rd32(game + 0x70)) return 0;
        const uint32_t r = e.imp(0xA57A, e.h(0x1020, e.rd32(a + 0xc), 0, 0, 0, 0, game), 0x1a, 9);
        if (e.rd8(data + 0x45)) return NE::lo(r, e.rd8(data + 0x45));
        return e.h(0xBAFF0, a, 0, 0, 0, 0, data);
    }
    if (m14 != 0xf6) return other;
    if (m18 == 0x2ad) return e.clrf(e.qbuf(e.rd32(q + 4), e.rd32(a + 0xc)), 0x1a, 6);
    if (m18 == 0x4e21) return e.setf(e.qbuf(e.rd32(q + 4), e.rd32(a + 0xc)), 0x1a, 8);
    return other;
}
}
