// 1.13c D2Game+0xB8B90 -- EAX = whatever the caller left (only AL is the result; the upper bytes come back from the last value computed), ECX = quest record, EDX = id, three stack arguments. For id 0x198 only, with the quest-flag buffer of the first stack argument (a unit) for the record's +4 game difficulty: AL = 1 when the import 0xA568(buffer, 0x1B, s) answers 0 for s = 0, 1 and 0xD, or, after any non-zero answer, when (buffer, 0x1A, 0xD) and (buffer, 0x1A, 0) both answer 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fb8b90(GE& e, uint32_t a, uint32_t q, uint32_t id, uint32_t s0, uint32_t s1, uint32_t)
{
    if (id != 0x198) return a & ~0xffu;
    const uint32_t unit = s0;
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(e.rd32(q + 4) + 0x6d));
    uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x1b, 0x0);
    if (!v) v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x1b, 0x1);
    if (!v) {
        v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x1b, 0xd);
        if (!v) return 1;
    }
    v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x1a, 0xd);
    if (!v) v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x1a, 0x0);
    if (v) return v & ~0xffu;
    return 1;
}
}
