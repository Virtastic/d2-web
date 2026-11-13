// 1.13c D2Game+0xE3050 -- quest event callback, no result. ECX = quest record {+4 game, +0x18 data}, EDX = event {+0 unit, +0xC player}.
// 0xA4C0(player) gives the player data; with the unit's byte +0x6D the buffer pd[0x10 + 4*difficulty]. If 0xA568(buf, 0x20, 0) == 1:
// the game's list pointer [[rec+4]+0x10F4] must have a non-zero word +8 (retail otherwise reports and exits), then 0xA57A([list+0xC], 0x20, 0xD)
// runs and data+1 is cleared.
#include <cstdlib>
#include "../fn/hookenv.h"
namespace d2g {
void fe3050(HookEnv& e, uint32_t q, uint32_t ev)
{
    const uint32_t unit = e.rd32(ev);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ev + 0xC));
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(unit + 0x6d));
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x20, 0) != 1) return;
    const uint32_t list = e.rd32(e.rd32(q + 4) + 0x10f4);
    if (!e.rd32(list + 8)) abort();   // retail's fatal tail: not captured
    e.h(0xA57A, 0, 0, 0, 0, 0, e.rd32(list + 0xC), 0x20, 0xD);
    e.wr8(e.rd32(q + 0x18) + 1, 0);
}
}
