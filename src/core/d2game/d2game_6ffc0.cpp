// 1.13c D2Game+0x6FFC0 -- PLRTRADE_AddItemTradePair: EAX = player unit, stdcall (game, id1, id2). With the player data's trade record at +0x5C present,
// allocates a 12-byte node from the game pool (game+0x1C, PlrTrade.cpp line 0xC58) and pushes it on the record's list head at +8: {id1, id2, old head}.
#include <cstring>
#include "d2game_6ffc0.h"
// 1.13c D2Game+0x6FFC0
void d2game_port_6ffc0(uint32_t player, uint32_t game, uint32_t id1, uint32_t id2, const D2Game6FFC0Env* env)
{
    auto rd = [](uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; };
    auto wr = [](uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); };
    const uint32_t pd = env->playerData(player);
    if (!pd) return;
    const uint32_t rec = rd(pd + 0x5c);
    if (!rec) return;
    const uint32_t node = env->alloc(rd(game + 0x1c), 0xc, 0xc58);
    wr(node, id1);
    wr(node + 4, id2);
    wr(node + 8, rd(rec + 8));
    wr(rec + 8, node);
}
