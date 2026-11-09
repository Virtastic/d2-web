// 1.13c D2Game+0xA58D0 (ECX = game, EDX = unit, ret 4 with one unused stack word)
#include "d2game_a58d0.h"
#include <cstring>

void d2game_a58d0(const D2GameA58D0Env& env, void* game, uint8_t* unit)
{
    uint32_t lo, hi;
    memcpy(&lo, unit + 0x20, 4); memcpy(&hi, unit + 0x24, 4);
    const uint64_t s = (uint64_t)lo * 0x6ac690c5u + hi;
    memcpy(unit + 0x20, &s, 8);
    if ((uint32_t)s % 100 >= 0x14) { env.defaultAction(game, 0x1e); return; }
    env.clearPath(unit, 0);
    uint32_t pkt[8] = { 8, (uint32_t)(uintptr_t)unit, 0, 0, 0, 0, 0, 0 };
    uint32_t flag = 0x64;
    uint32_t cls; memcpy(&cls, unit + 4, 4);
    if (const uint8_t* rec = env.monStats(cls))
        if (rec[0x105] & env.flagMask) flag = 0x65;
    pkt[5] = flag << 8;
    env.sendFrame(game, pkt, 1);
}
