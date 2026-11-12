// 1.13c D2Game+0xB8810 -- EAX = whatever the caller left (only AL is the result; the upper bytes come back from the last value computed), ECX = quest record, EDX = id, three stack arguments. AL = 1 only for id 0xF6 when the import 0xA568(second stack argument, 0x1B, s) answers 0 for s = 0 and s = 1, the record's +9 byte is set, its +0xC byte is 1 and the +0x1C byte of its +0x18 data is 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fb8810(GE& e, uint32_t a, uint32_t q, uint32_t id, uint32_t s0, uint32_t s1, uint32_t)
{
    uint32_t v = a;
    if (id == 0xf6) {
        v = e.h(0xA568, 0, 0, 0, 0, 0, s1, 0x1b, 0x0);
        if (!v) v = e.h(0xA568, 0, 0, 0, 0, 0, s1, 0x1b, 0x1);
        if (!v) {
            v = e.rd8(q + 9);
            if (v && e.rd8(q + 0xc) == 1) {
                v = e.rd32(q + 0x18);
                if (!e.rd8(v + 0x1c)) return (v & ~0xffu) | 1;
            }
        }
    }
    return v & ~0xffu;
}
}
