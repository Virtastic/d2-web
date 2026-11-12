// 1.13c D2Game+0x60660 -- EAX = whatever the caller left (only AL is the result; the upper bytes come back from the last value computed), ECX = quest record, EDX = id, three stack arguments. For id 0x96 only, with Q = the record's +0xE0 word and the second stack argument as buffer: the import 0xA568(buffer, Q, 0) answering 1 gives AL = 0. With the record's +0xC byte 1, (buffer, Q, 1) answering 0 gives AL = 1. Otherwise AL = 1 exactly when (buffer, Q, 1) answers 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f60660(GE& e, uint32_t a, uint32_t q, uint32_t id, uint32_t s0, uint32_t s1, uint32_t)
{
    if (id != 0x96) return a & ~0xffu;
    const uint32_t Q = e.rd32(q + 0xe0);
    uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, s1, Q, 0);
    if (v == 1) return 0;
    if (e.rd8(q + 0xc) == 1) {
        v = e.h(0xA568, 0, 0, 0, 0, 0, s1, Q, 1);
        if (!v) return 1;
    }
    v = e.h(0xA568, 0, 0, 0, 0, 0, s1, Q, 1);
    return v == 1 ? 1 : v & ~0xffu;
}
}
