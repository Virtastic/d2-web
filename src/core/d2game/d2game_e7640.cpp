// 1.13c D2Game+0xE7640 -- quest npc event handler: the quest 0x14 flags gate it, then the npc class decides between state 3 and the state 1 flag set
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fe7640(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t game = e.rd32(a);
    const uint32_t buf = e.qbuf(game, e.rd32(a + 0xc));
    uint32_t eax = e.fl(buf, 0x14, 0); if (eax) return eax;
    eax = e.fl(buf, 0x14, 1); if (eax) return eax;
    const uint32_t npc = e.rd32(a + 8);
    eax = e.rd32(e.imp(0xA53E, npc ? e.rd32(npc + 4) : ~0u) + 0x80);
    if (eax == 0x20343367) { e.h(0x35EB0, 3); e.wr8(q + 0xb, 3); return e.cb(e.rd32(q + 4), 0xE5F00, 0, 0); }
    if (eax != 0x2034336a) return eax;
    e.wr8(q + 0xb, 1); e.cb(e.rd32(q + 4), 0xE5F00, 0, 0);
    eax = e.h(0x35EB0, 1);
    eax = e.fl(buf, 0x14, 6); if (eax) return eax;
    e.setf(buf, 0x14, 6);
    const uint32_t u = e.rd32(a + 0xc);
    return e.h(0xDFFD0, 0, 0, 0, 0, u, 0x48, u);
}
}
