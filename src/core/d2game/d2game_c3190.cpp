// 1.13c D2Game+0xC3190 -- ECX = game, EDX = unit, one ignored stack argument: the D2Common import 0xA4C0(unit) gives the player data; its quest-flag buffer for the game's difficulty (byte +0x6D; buffers from +0x10) goes to the import 0xA568(buffer, 0xC, 0xD). Returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc3190(GE& e, uint32_t game, uint32_t unit, uint32_t)
{
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    e.h(0xA568, 0, 0, 0, 0, 0, e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d)), 0xc, 0xd);
    return 0;
}
}
