// 1.13c D2Game+0xE7940 -- quest 0xf5 event handler: the 0x21f..0x224 messages set the quest 0x12 flags 2, 6, 3, 4, 5, 7
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fe7940(NE& e, uint32_t q, uint32_t a)
{
    const uint32_t game = e.rd32(a), unit = e.rd32(a + 0xc);
    const uint32_t w18 = NE::rd16(a + 0x18), w14 = NE::rd16(a + 0x14);
    const uint32_t eax = e.imp(0xA4C0, unit);
    const uint32_t buf = e.rd32(eax + 0x10 + 4 * e.rd8(game + 0x6d));
    if (w14 != 0xf5) return eax;
    const uint32_t idx = (w18 - 0x21f) >> 0;
    if (idx > 5) return idx;
    static const uint8_t F[6] = {2, 6, 3, 4, 5, 7};
    if (idx == 0) {
        e.h(0x35EB0, 2);
        const uint32_t r = e.setf(buf, 0x12, 2);
        e.wr8(e.rd32(q + 0x18) + 1, 1);
        e.wr32(q + 0xa8, 0xB2000000u | 0xE6D90);
        return r;
    }
    return e.setf(buf, 0x12, F[idx]);
}
}
