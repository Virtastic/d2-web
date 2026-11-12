// 1.13c D2Game+0x61B70 -- EAX = whatever the caller left (only AL is the result; the upper bytes come back from the last value computed), ECX = quest record, EDX = id, three stack arguments. For id 0x9A only, with the second stack argument as buffer and the first as unit: the import 0xA568(buffer, 3, 0) answering 1 gives AL = 0. With the record's +0xC byte 1, (buffer, 3, 1) answering 0 gives AL = 1. Otherwise the D2Common import 0xA3E2(unit, 0xC, 0) below 8 (signed) gives AL = 0; else AL = 1 exactly when the helper 0xE660 (EAX = unit, EBX = the result of 0xDF780 with EAX = unit, stack 0x206D6468) answers non-zero.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f61b70(GE& e, uint32_t a, uint32_t q, uint32_t id, uint32_t s0, uint32_t s1, uint32_t)
{
    if (id != 0x9a) return a & ~0xffu;
    uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, s1, 0x3, 0x0);
    if (v == 1) return 0;
    if (e.rd8(q + 0xc) == 1) {
        v = e.h(0xA568, 0, 0, 0, 0, 0, s1, 0x3, 0x1);
        if (!v) return 1;
    }
    v = e.h(0xA3E2, 0, 0, 0, 0, 0, s0, 0xc, 0);
    if ((int32_t)v < 8) return v & ~0xffu;
    const uint32_t x = e.h(0xDF780, s0, 0, 0, 0, 0);
    v = e.h(0xE660, s0, 0, 0, x, 0, 0x206d6468);
    if (!v) return 0;
    return (v & ~0xffu) | 1;
}
}
