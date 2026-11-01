// 1.13c D2Game+0x4EF70: Five stack words (ret 0x14), a0 and a1 used: a0 = owner, a1 = target (its [0] is 0 or 1). Reads a stat (#A802 id 0x96), caps it (0x32 for type 0, 0x32/0x5A for type 1 by 0x22A80(a1, 0xC)), builds a 9-word request on the stack and hands it to 0xA00B0 (EAX; the capture cannot read a block through EAX, so the request words are not logged), then applies the negated value to two stats (#A76C ids 0x44, 0x45) of the result and calls #A59E(a1, string, 0xB62).
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4ef70(uint32_t a0, uint32_t a1, const D2Seam* e)
{
    if (!a0 || !a1) return 0;
    int32_t v = (int32_t)e->call(0xA802, 0, 0, 0, 0, 0, a0, 0x96, 0);
    if (!v) return 0;
    const uint32_t type = d2rd32(a1);
    if (type == 0) {
        if (v >= 0x32) v = 0x32;
    } else if (type == 1) {
        if (e->call(0x22A80, a1, 0, 0, 0, 0, 0xC)) { if (v >= 0x32) v = 0x32; }
        else if (v >= 0x5A) v = 0x5A;
    } else return 0;
    uint32_t req[9] = { a0, a1, 0, 1, 0x2EE, 0x43, (uint32_t)-v, 0x18, 0 };
    const uint32_t r = e->call(0xA00B0, (uint32_t)(uintptr_t)req);
    if (!r) return 0;
    e->call(0xA76C, 0, 0, 0, 0, 0, r, 0x44, (uint32_t)-v, 0);
    e->call(0xA76C, 0, 0, 0, 0, 0, r, 0x45, (uint32_t)-v, 0);
    e->call(0xA59E, 0, 0, 0, 0, 0, a1, 0xB20FC1DCu, 0xB62);
    return 1;
}
