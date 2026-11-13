// 1.13c D2Game+0xE89A0 -- quest 0x15 event handler: the 0x252 talk closes the dialogue, the 0x272 message runs the five class callbacks and the quest finish callback
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fe89a0(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t game = e.rd32(a), unit = e.rd32(a + 0xc);
    const uint32_t w18 = NE::rd16(a + 0x18), w14 = NE::rd16(a + 0x14);
    const uint32_t buf = e.qbuf(game, unit);
    uint32_t eax = e.fl(buf, 0x15, 0);
    if (eax == 1) return eax;
    if (w14 == 0xff) {
        if (w18 != 0x252) return eax;
        eax = e.h(0x338D0, game, 0, 0, 0, 0, 0x11, 0xd);
        e.h(0x35EB0, eax == 1 ? 2 : 3);
        e.wr8(e.rd32(q + 0x18) + 4, 1);
        return e.h(0x35910, e.rd32(a + 8), unit, 0, 0, 0, game);
    }
    if (w14 != 0xf5 || w18 != 0x272) return eax;
    eax = e.fl(buf, 0x15, 4);
    if (!eax) return eax;
    eax = e.fl(buf, 0x12, 0);
    if (eax) {
        static const uint32_t K[5] = {0x20796571, 0x20726871, 0x20726271, 0x20316671, 0x20326671};
        for (const uint32_t k : K) eax = e.h(0x366A0, k, game, 0, 0, unit);
    }
    eax = e.fl(buf, 0x15, 0xd);
    if (eax && e.rd8(q + 0xc) != 7) {
        e.h(0x33920, game, 0, 0, 0, 0, 0x15, 0xd);
        e.wr8(q + 0x14, 0); e.wr8(q + 0xb, 0xd);
        e.h(0x35EB0, 7);
    }
    if (e.imp(0xF8D14, e.rd32(q + 0xf0))) return 0;
    eax = e.h(NE::handlerRva(e.rd32(q + 0xf0)), 0, q);
    e.setf(buf, 0x15, 0);
    e.clrf(buf, 0x15, 4);
    const uint32_t u = e.rd32(a + 0xc);
    e.h(0x33880, q + 0x1c, 0, 0, 0, u ? e.rd32(u + 0xc) : ~0u);
    return e.h(0x35910, e.rd32(a + 8), e.rd32(a + 0xc), 0, 0, 0, e.rd32(a));
}
}
