// 1.13c D2Game+0xB8FE0 -- ECX unused, EDX = event: a zero +0x70 word in the event's +0 game returns 0. Otherwise, with the quest-flag buffer of the event's +0xC unit for the game's difficulty (player data from the D2Common import 0xA4C0): a zero answer from the import 0xA568(buffer, 0x1A, 9) is returned; a non-zero answer from 0xA568(buffer, 0x1C, 0) is returned; otherwise the result of the import 0xA586(buffer, 0x1A, 9).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fb8fe0(GE& e, uint32_t, uint32_t ev)
{
    const uint32_t game = e.rd32(ev);
    if (!e.rd32(game + 0x70)) return 0;
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ev + 0xc));
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x1a, 9);
    if (!v) return 0;
    v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x1c, 0);
    if (v) return v;
    return e.h(0xA586, 0, 0, 0, 0, 0, buf, 0x1a, 9);
}
}
