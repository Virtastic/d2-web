// 1.13c D2Game+0x63F90 -- npc message handler of the quest 6: the four npcs and their messages
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f63f90(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t m14 = NE::rd16(a + 0x14), m18 = NE::rd16(a + 0x18), pl = e.rd32(a + 0xc), uid = (pl ? e.rd32(pl + 0xc) : 0xffffffffu), data = e.rd32(q + 0x18);
    if (m14 == 0x109) {
        if (m18 == 0xa6) {
            e.wr8(q + 0xc, 2);
            e.say(e.rd32(q + 4), 2);
            e.wr8(data + 0x195, 1);
            e.cb(e.rd32(q + 4), 0x60D00);
            return e.h(0x35910, e.rd32(a + 8), e.rd32(a + 0xc), 0, 0, 0, e.rd32(a));
        }
        if (m18 == 0xb8) return e.h(0x33830, uid, data);
        return uid;
    }
    if (m14 == 0x9b) {
        if (m18 != 0xb7) return uid;
        e.h(0x35910, e.rd32(a + 8), pl, 0, 0, 0, e.rd32(a));
        const uint32_t buf = e.qbuf(e.rd32(a), e.rd32(a + 0xc));
        const uint32_t eax = e.fl(buf, 6, 1);
        if (!eax) return eax;
        if (e.fl(buf, 6, 0xd)) {
            e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 0xd);
            e.h(0x35EB0, 5);
            e.h(0x33920, e.rd32(q + 4), 0, 0, 0, 0, 6, 0xd);
        }
        e.clrf(buf, 6, 1); e.setf(buf, 6, 0);
        const uint32_t p2 = e.rd32(a + 0xc);
        e.h(0x33880, q + 0x1c, 0, 0, 0, (p2 ? e.rd32(p2 + 0xc) : 0xffffffffu));
        return e.h(0x34C00, e.rd32(a + 0xc), 0, 0, 0, 0, e.rd32(q + 4));
    }
    if (m14 == 0x94) return m18 == 0xb3 ? e.h(0x33830, uid, data + 0x84) : uid;
    if (m14 == 0x96 && m18 == 0xb5) return e.h(0x33830, uid, data + 0x108);
    return uid;
}
}
