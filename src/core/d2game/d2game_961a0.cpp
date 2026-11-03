// 1.13c D2Game+0x961A0: ecx = c, edx = unit, stack a0, a1 (ret 8); always returns 1. With a list at unit +0x60 the two ends e1 = 0xA7AE(list, 4), e2 = 0xA7AE(list, 5) must be
// distinct and non-null, both pass 0xA8CE and both 0xA430(end, 0x2D); any failure sets bit 0x40 of unit +0xC4. Otherwise (and with no list) the bit follows bit 8 of unit +0x38:
// set when it is 1, cleared when 0. 0x95690(ecx = c, edx = unit; a0, a1) closes it.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_961a0(uint32_t c, uint32_t unit, uint32_t a0, uint32_t a1, const D2Seam* e)
{
    const uint32_t list = d2rd32(unit + 0x60);
    bool flagOnly = true;
    if (list) {
        const uint32_t e1 = e->call(0xA7AE, 0, 0, 0, 0, 0, list, 4), e2 = e->call(0xA7AE, 0, 0, 0, 0, 0, list, 5);
        flagOnly = e1 != e2 && e1 && e2 && e->call(0xA8CE, 0, 0, 0, 0, 0, e1) && e->call(0xA8CE, 0, 0, 0, 0, 0, e2)
            && e->call(0xA430, 0, 0, 0, 0, 0, e1, 0x2D) && e->call(0xA430, 0, 0, 0, 0, 0, e2, 0x2D);
        if (!flagOnly) d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) | 0x40);
    }
    if (flagOnly) {
        if ((((int32_t)d2rd32(unit + 0x38) >> 8) & 1) == 0) d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) & 0xFFFFFFBFu);
        else d2wr32(unit + 0xC4, d2rd32(unit + 0xC4) | 0x40);
    }
    e->call(0x95690, 0, c, unit, 0, 0, a0, a1);
    return 1;
}
