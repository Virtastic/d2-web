// 1.13c D2Game+0x56840 -- EAX = whatever the caller left (only AL is the result; the upper bytes come back from the last value computed), ECX = quest record, EDX = id, three stack arguments. For id 0x200 only, with Q = the record's +0xE0 word and the second stack argument as buffer. With the record's +0xC byte 1: when the import 0xA568(buffer, 0x25, 0) and (buffer, 0x25, 1) both answer 0, AL = 0; otherwise AL = 1 exactly when (buffer, Q, 0) and (buffer, Q, 1) both answer 0. With any other +0xC byte: AL = 1 exactly when (buffer, Q, 0xD) answers non-zero and (buffer, 0x26, 4) answers 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f56840(GE& e, uint32_t a, uint32_t q, uint32_t id, uint32_t s0, uint32_t s1, uint32_t)
{
    if (id != 0x200) return a & ~0xffu;
    const uint32_t Q = e.rd32(q + 0xe0);
    uint32_t v;
    if (e.rd8(q + 0xc) == 1) {
        v = e.h(0xA568, 0, 0, 0, 0, 0, s1, 0x25, 0x0);
        if (!v) {
            v = e.h(0xA568, 0, 0, 0, 0, 0, s1, 0x25, 0x1);
            if (!v) return 0;
        }
        v = e.h(0xA568, 0, 0, 0, 0, 0, s1, Q, 0);
        if (v) return v & ~0xffu;
        v = e.h(0xA568, 0, 0, 0, 0, 0, s1, Q, 1);
        if (v) return v & ~0xffu;
        return 1;
    }
    v = e.h(0xA568, 0, 0, 0, 0, 0, s1, Q, 0xd);
    if (!v) return 0;
    v = e.h(0xA568, 0, 0, 0, 0, 0, s1, 0x26, 0x4);
    if (v) return v & ~0xffu;
    return 1;
}
}
