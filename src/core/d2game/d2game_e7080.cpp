// 1.13c D2Game+0xE7080 -- quest 0x16 npc event handler: the 0x274 talk closes the dialogue, the 0x291..0x297 messages move the state to 7 when the 0xd flag is set
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fe7080(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t game = e.rd32(a), unit = e.rd32(a + 0xc);
    const uint32_t w14 = NE::rd16(a + 0x14), w18 = NE::rd16(a + 0x18);
    const uint32_t buf = e.qbuf(game, unit);
    uint32_t eax = e.fl(buf, 0x16, 0xb);
    if (!eax) { eax = e.fl(buf, 0x16, 0); if (eax) return eax; }
    if (w14 == 0xff && w18 == 0x274) {
        eax = e.h(0x338D0, game, 0, 0, 0, 0, 0x11, 0xd);
        e.h(0x35EB0, eax == 1 ? 2 : 3);
        e.wr8(e.rd32(q + 0x18) + 3, 1);
    } else {
        if (w18 < 0x291 || w18 > 0x297) return eax;
        eax = e.fl(buf, 0x16, 0xb);
        if (eax) {
            eax = e.fl(buf, 0x16, 0xd);
            if (eax) {
                e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 0xd); e.wr8(q + 0xc, 7);
                e.say(e.rd32(q + 4), 7);
                eax = 0;
            }
            e.clrf(buf, 0x16, 0xb);
            const uint32_t u = e.rd32(a + 0xc);
            e.h(0x33880, q + 0x1c, 0, 0, 0, u ? e.rd32(u + 0xc) : ~0u);
        }
    }
    return e.h(0x35910, e.rd32(a + 8), e.rd32(a + 0xc), 0, 0, 0, game);
}
}
