// 1.13c D2Game+0xE7560 -- quest event handler: the 0x52 step opens state 1, the 0x4b talk step opens state 2 when the quest flags are clear
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fe7560(NE& e, uint32_t z, uint32_t q, uint32_t a)
{
    uint32_t eax = z;
    const uint32_t game = e.rd32(a);
    eax = e.rd32(a + 0x14);
    if (e.rd32(a + 0x18) == 0x52 && e.rd8(q + 9) && !e.rd8(q + 0xc)) { e.wr8(q + 0xc, 1); e.wr32(q + 0xa8, 0xB2000000u | 0xE69E0); }
    if (eax != 0x4b) return eax;
    if (NE::rd16(q + 0x9c)) { const uint32_t u = e.rd32(a + 0xc); eax = e.h(0x33830, u ? e.rd32(u + 0xc) : ~0u, q + 0x1c); }
    eax = NE::lo(eax, e.rd8(q + 0xc));
    const uint32_t c = e.rd8(q + 0xc);
    if (c <= 1 || c > 3) return eax;
    const uint32_t buf = e.qbuf(game, e.rd32(a + 0xc));
    eax = e.fl(buf, e.rd32(q + 0xe0), 0); if (eax == 1) return eax;
    eax = e.fl(buf, e.rd32(q + 0xe0), 4); if (eax == 1) return eax;
    if (e.rd8(q + 0xb) < 3) { e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 2); }
    eax = e.h(0x338D0, game, 0, 0, 0, 0, 0x11, 0xd);
    eax = e.h(0x35EB0, eax == 1 ? 4 : 5);
    return e.cb(e.rd32(q + 4), 0xE3A80, 0, 0);
}
}
