// 1.13c D2Game+0xBA330 -- ECX = quest record, EDX = event. With the quest-flag buffer of the event's +0xC unit for the record's +4 game difficulty: a non-zero answer from the import 0xA568(buffer, 0x1B, s) for s = 0, 0xD or 1 is returned. Then a zero +9 record byte returns 0, a +0xB byte of 0xD returns the +9 byte and a non-zero +0xC byte returns it; otherwise the result of the state helper 0x35EB0 (EAX = 1, ESI = record) is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fba330(GE& e, uint32_t q, uint32_t ev)
{
    const uint32_t unit = e.rd32(ev + 0xc);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(e.rd32(q + 4) + 0x6d));
    uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x1b, 0x0);
    if (v) return v;
    v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x1b, 0xd);
    if (v) return v;
    v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x1b, 0x1);
    if (v) return v;
    v = e.rd8(q + 9);
    if (!v || e.rd8(q + 0xb) == 0xd) return v;
    v = e.rd8(q + 0xc);
    if (v) return v;
    return e.h(0x35EB0, 1, 0, 0, 0, q);
}
}
