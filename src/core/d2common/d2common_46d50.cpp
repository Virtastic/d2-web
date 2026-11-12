// 1.13c D2Common+0x46D50 -- seven stack arguments, ret 28: works on the seventh argument when it is an item unit (type 4), else on the first. For each of the packed stats 0x490000 then 0x480000: when the unit has a stat list (+0x5C) holding the stat (0x382B0 with EAX = the stat array at list +0x24 and EDI = the stat answers an index, negative = absent) with a non-zero value (+4 of the 8-byte entry), calls 0x381A0 with the first dword of the list, adds the negated value through 0x39F70 (EAX = list, EDI = stat, stack = amount, 0), calls 0x38550 (EAX = stat, EBX = list) when the list flags at +0x10 have the top bit and the dword at +8 is zero, and calls it again with the re-read list when the unit type is 0. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
uint32_t f46d50(GE& e, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6)
{
    (void)s1; (void)s2; (void)s3; (void)s4; (void)s5;
    const uint32_t u = (s6 && e.rd32(s6) == 4) ? s6 : s0;
    for (const uint32_t st : { 0x490000u, 0x480000u }) {
        const uint32_t l = e.rd32(u + 0x5c);
        if (!l) continue;
        e.ed(st);
        const int32_t ix = (int32_t)e.h(0x382b0, l + 0x24);
        e.edi(0x382b0, st);
        if (ix < 0) continue;
        const uint32_t p = e.rd32(l + 0x24) + (uint32_t)ix * 8;
        if (!p) continue;
        const uint32_t n = 0u - e.rd32(p + 4);
        if (!n) continue;
        e.h(0x381a0, 0, 0, 0, 0, 0, e.rd32(l));
        e.ed(st);
        e.h(0x39f70, l, 0, 0, 0, 0, n, 0);
        e.edi(0x39f70, st);
        if ((int32_t)e.rd32(l + 0x10) < 0 && !e.rd32(l + 8)) e.h(0x38550, st, 0, 0, l);
        if (e.rd32(u) == 0) e.h(0x38550, st, 0, 0, e.rd32(u + 0x5c));
    }
    return 1;
}
}
