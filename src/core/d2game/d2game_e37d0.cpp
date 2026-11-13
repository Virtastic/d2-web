// 1.13c D2Game+0xE37D0 -- ECX = game, EDX = unit, one ignored stack argument. The unit's quest-flag buffer for the game's difficulty (player data from the D2Common import 0xA4C0, byte +0x6D, buffers from +0x10) is read: if 0xA568(buffer, 0x15, 0) or 0xA568(buffer, 0x15, 4) answers 1, that 1 is returned. Otherwise the unit's room (0xA478), the room's level (0xA556) and the level's record byte (0xA56E) are asked; a zero from the first two is returned, a record byte other than 2 returns the 0xA56E answer. With the byte 2, 0xA57A(buffer, 0x15, X) runs with X = 0 when 0xA568(buffer, 0x12, 0) is non-zero, else 4, then 0xA57A(buffer, 0x15, 0xd); its answer is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe37d0(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x15, 0);
    if (v == 1) return v;
    v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x15, 4);
    if (v == 1) return v;
    v = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (!v) return v;
    v = e.h(0xA556, 0, 0, 0, 0, 0, v);
    if (!v) return v;
    v = e.h(0xA56E, 0, 0, 0, 0, 0, v);
    if ((v & 0xff) != 2) return v;
    const uint32_t f = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x12, 0);
    e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0x15, f ? 0u : 4u);
    return e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0x15, 0xd);
}
}
