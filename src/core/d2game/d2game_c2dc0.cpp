// 1.13c D2Game+0xC2DC0 -- ECX = quest record, EDX = event. With the quest-flag buffer of the event's +0xC unit for the event's +0 game difficulty and Q = the record's +0xE0 word: a non-zero answer from the import 0xA568(buffer, Q, 0) or (buffer, Q, 0xF), or an answer other than 1 from (buffer, 0xD, 2), is returned. Otherwise the record's +0xB byte becomes 2 and its +0xC byte 1; returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc2dc0(GE& e, uint32_t q, uint32_t ev)
{
    const uint32_t unit = e.rd32(ev + 0xc), game = e.rd32(ev);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, buf, e.rd32(q + 0xe0), 0);
    if (v) return v;
    v = e.h(0xA568, 0, 0, 0, 0, 0, buf, e.rd32(q + 0xe0), 0xf);
    if (v) return v;
    v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0xd, 0x2);
    if (v != 1) return v;
    e.wr8(q + 0xb, 2);
    e.wr8(q + 0xc, 1);
    return 1;
}
}
