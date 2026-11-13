// 1.13c D2Game+0xE7770 -- quest npc event handler: the class 0x20333367 npc opens state 4, sets the 0x13/9 flag once and moves on to state 3
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fe7770(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t npc = e.rd32(a + 8);
    uint32_t eax = e.imp(0xA53E, npc ? e.rd32(npc + 4) : ~0u);
    if (e.rd8(q + 9) && e.rd32(eax + 0x80) == 0x20333367) {
        e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 4);
        e.cb(e.rd32(q + 4), 0xE6130, 0, 0);
        const uint32_t buf = e.qbuf(e.rd32(a), e.rd32(a + 0xc));
        eax = e.fl(buf, 0x13, 9);
        if (!eax) { e.setf(buf, 0x13, 9); const uint32_t u = e.rd32(a + 0xc); e.h(0xDFFD0, 0, 0, 0, 0, u, 0x41, u); }
        e.h(0x35EB0, 4);
        e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 3);
        e.cb(e.rd32(q + 4), 0xE6130, 0, 0);
    }
    return e.cb(e.rd32(q + 4), 0xE46F0, 0, 0);
}
}
