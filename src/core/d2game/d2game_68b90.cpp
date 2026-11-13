// 1.13c D2Game+0x68B90 -- quest 0x25 npc arrival: records the npc position; an active record moves to state 5, marks the data, sets the flag 0xD, runs the three player steps and arms the timer; then the spawn step and, unless the data says done, the sound 7
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f68b90(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t data = e.rd32(q + 0x18);
    e.imp(0xA544, e.rd32(a + 8), data + 0x110);
    if (e.rd8(q + 9)) {
        if (e.trace) e.h(0xCBE70, 0, 0, 0xB2000000u | 0xFB2E0, 0, 0, e.rd32(q + 4), 4);
        e.wr32(q + 0xa8, 0); e.wr8(q + 0xc, 5);
        e.say(e.rd32(q + 4), 5);
        e.wr8(data + 0x11a, 1);
        e.h(0x63350, 0, 0, 0, 0, q);
        e.wr8(data + 0x118, 1);
        const uint32_t fid = e.rd32(q + 0xe0);
        e.wr32(q + 0xc0, 0);
        const uint32_t hold = e.rd32(e.rd32(a) + 0x10f4);
        e.setf(e.rd32(hold + 0xc), fid, 0xd);            // the vectors keep the holder's count set: retail asserts on a zero one
        e.cb(e.rd32(a), 0x60F70, 0, e.rd32(a + 8));
        e.h(0x63270, 0, 0, 0, e.rd32(a), 0, data + 0x8c, e.rd32(q + 0xe0), 0x25);
        e.cb(e.rd32(a), 0x64150, 0, e.rd32(a + 8));
        e.cb(e.rd32(a), 0x62760, 0, e.rd32(a + 8));
        e.h(0x33AF0, 0, 0, 0, 7, 0, q, 0xB2000000u | 0x63210);
    }
    const uint32_t r = e.h(0x65E90, 0, 0, 0, 0, 0, q, data);
    if (e.rd8(data + 0x119)) return NE::lo(r, e.rd8(data + 0x119));
    const uint32_t game = e.rd32(q + 4);
    return e.h(0x8EA80, 0, game, e.rd32(a + 8), 0, 0, 7, e.rd32(game + 0xa8) + 0xa, 0, 0, 0);
}
}
