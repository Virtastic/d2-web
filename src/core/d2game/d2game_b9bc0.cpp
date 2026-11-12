// 1.13c D2Game+0xB9BC0 -- ECX = game, EDX = unit, one ignored stack argument: the D2Common import 0xA4C0(unit) gives the player data, whose quest-flag buffer for the game's difficulty (byte +0x6D; buffers from +0x10) is used. If the import 0xA568(buffer, 0x1a, 0) answers 1 nothing more happens; otherwise 0xA57A(buffer, 0x1a, 0xe) sets the state and the notifier 0x34AA0 runs with (0x17, 0), EAX = the setter's result and ESI = unit. Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fb9bc0(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x1a, 0) == 1) return 0;
    const uint32_t v = e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0x1a, 0xe);
    e.h(0x34AA0, v, 0, 0, 0, unit, 0x17, 0);
    return 0;
}
}
