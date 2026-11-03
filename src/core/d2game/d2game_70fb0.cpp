// 1.13c D2Game+0x70FB0 -- PlrTrade init: EBX = unit, EDI = game; returns AL. When the player data's trade record (+0x5C) is absent, allocates the
// 0x18-byte record and its 0x2000-byte item buffer from the game pool (PlrTrade.cpp lines 0x803, 0x804), serialises the inventory into the buffer
// (0x537D0), stores the end pointer, clears +8/+0xC/+0x14, stores stat 0xE at +0x10 and hangs the record on the player data. A non-positive
// end pointer is the fatal path at line 0x806.
#include <cstring>
#include "d2game_70fb0.h"
// 1.13c D2Game+0x70FB0
uint32_t d2game_port_70fb0(uint32_t unit, uint32_t game, const D2Game70FB0Env* env)
{
    auto rd = [](uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; };
    auto wr = [](uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); };
    const uint32_t pd = env->playerData(unit);
    if (rd(pd + 0x5c)) return 0;
    const uint32_t rec = env->alloc(rd(game + 0x1c), 0x18, 0x803);
    const uint32_t buf = env->alloc(rd(game + 0x1c), 0x2000, 0x804);
    wr(rec + 4, buf);
    const uint32_t end = env->writeItems(buf, rd(unit + 0x60), game, unit, 0x2000);
    wr(rec, end);
    if ((int32_t)end <= 0) env->fatal(0x806);
    wr(rec + 8, 0); wr(rec + 0xc, 0); wr(rec + 0x14, 0);
    wr(rec + 0x10, env->getStat(unit, 0xe, 0));
    wr(pd + 0x5c, rec);
    return 1;
}
