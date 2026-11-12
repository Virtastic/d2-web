// 1.13c D2Game+0x56EB0 -- ECX = quest record, EDX = event: the D2Common import 0xA4C0 of the event's +0xC unit gives the player data; with the quest-flag buffer for the difficulty of the event's +0 game, the import 0xA568(buffer, the record's +0xE0 quest id, 0) is asked. A zero answer is returned; otherwise the import 0xA57A(buffer, quest id, 0xA) runs and its result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f56eb0(GE& e, uint32_t q, uint32_t ev)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ev + 0xc));
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(e.rd32(ev) + 0x6d));
    if (!e.h(0xA568, 0, 0, 0, 0, 0, buf, e.rd32(q + 0xe0), 0)) return 0;
    return e.h(0xA57A, 0, 0, 0, 0, 0, buf, e.rd32(q + 0xe0), 0xa);
}
}
