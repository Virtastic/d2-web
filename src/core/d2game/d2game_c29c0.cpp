// 1.13c D2Game+0xC29C0 -- ECX unused, EDX = event: the D2Common import 0xA4C0 of the event's +0xC unit gives the player data; its quest-flag buffer for the game's difficulty (game = event +0, byte +0x6D; buffers from +0x10) goes to the import 0xA586(buffer, 0xa, 9), whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc29c0(GE& e, uint32_t, uint32_t ev)
{
    const uint32_t game = e.rd32(ev);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ev + 0xc));
    return e.h(0xA586, 0, 0, 0, 0, 0, e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d)), 0xa, 9);
}
}
