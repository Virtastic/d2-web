// 1.13c D2Game+0xC44E0 -- ECX = quest record, EDX = event. With the quest-flag buffer of the event's +0xC unit for the event's +0 game difficulty (player data from the D2Common import 0xA4C0): an answer other than 1 from the import 0xA568(buffer, 0x8, 0) is returned. Otherwise the holder at +0x10F4 of the record's +4 game (its +8 word must be set) gives the buffer at its +0xC and the result of the import 0xA57A(that buffer, 0x8, 0xD) is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc44e0(GE& e, uint32_t q, uint32_t ev)
{
    const uint32_t unit = e.rd32(ev + 0xc), game = e.rd32(ev);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    const uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x8, 0x0);
    if (v != 1) return v;
    const uint32_t hd = e.rd32(e.rd32(q + 4) + 0x10f4);
    return e.h(0xA57A, 0, 0, 0, 0, 0, e.rd32(hd + 0xc), 0x8, 0xd);
}
}
