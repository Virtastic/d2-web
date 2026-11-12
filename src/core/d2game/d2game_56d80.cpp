// 1.13c D2Game+0x56D80 -- EAX = whatever the caller left (only AL is the result; the upper bytes come back from the last value computed), ECX = quest record, EDX = id, three stack arguments. For id 0x1FF only, with Q = the record's +0xE0 word and the second stack argument as buffer: AL = 1 when the import 0xA568(buffer, Q, 1) answers non-zero and (buffer, Q, 5) answers 0. Otherwise AL = 1 exactly when the record's +9 byte is set, its +0xC byte is 1 and (buffer, Q, 0) and (buffer, Q, 1) both answer 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f56d80(GE& e, uint32_t a, uint32_t q, uint32_t id, uint32_t s0, uint32_t s1, uint32_t)
{
    if (id != 0x1ff) return a & ~0xffu;
    const uint32_t Q = e.rd32(q + 0xe0);
    uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, s1, Q, 1);
    if (v) {
        v = e.h(0xA568, 0, 0, 0, 0, 0, s1, Q, 5);
        if (!v) return 1;
    }
    if (!e.rd8(q + 9) || e.rd8(q + 0xc) != 1) return v & ~0xffu;
    v = e.h(0xA568, 0, 0, 0, 0, 0, s1, Q, 0);
    if (v) return v & ~0xffu;
    v = e.h(0xA568, 0, 0, 0, 0, 0, s1, Q, 1);
    if (v) return v & ~0xffu;
    return 1;
}
}
