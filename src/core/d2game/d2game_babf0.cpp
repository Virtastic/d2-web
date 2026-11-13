// 1.13c D2Game+0xBABF0 -- quest 0x1A start: state 4, records the npc, marks quest 0x19 on the player, runs three player steps and arms the timer once
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fbabf0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18);
    e.wr32(q + 0xa8, 0); e.wr8(q + 0xc, 4);
    e.say(e.rd32(q + 4), 4);
    e.wr32(q + 0xc0, 0);
    e.wr32(data + 0x14, e.rd32(a + 8));
    uint32_t r = NE::lo(e.rd32(a + 8), e.rd8(q + 9));
    if (e.rd8(q + 9)) {
        const uint32_t pl = e.rd32(a + 0xc);
        if (pl) {
            const uint32_t buf = e.qbuf(e.rd32(a), pl);
            if (!e.fl(buf, 0x19, 0) && !e.fl(buf, 0x19, 1)) {
                e.setf(buf, 0x19, 0xd); e.setf(buf, 0x19, 1);
                e.imp(0xA574, buf, e.rd32(q + 0xe0));
            }
        }
        e.wr8(data + 0x10, 1);
        e.imp(0xA544, e.rd32(a + 8), data + 4);
        e.cb(e.rd32(a), 0xB93B0, 0, e.rd32(a + 8));
        e.cb(e.rd32(a), 0xBA180, 0, e.rd32(a + 8));
        r = e.cb(e.rd32(a), 0xB9CD0, 0, e.rd32(a + 8));
    }
    if (e.rd8(data + 0xd)) return NE::lo(r, e.rd8(data + 0xd));
    e.wr8(data + 0xd, 1);
    return e.h(0x33AF0, 0, 0, 0, 3, 0, q, 0xB2000000u | 0xBA960);
}
}
