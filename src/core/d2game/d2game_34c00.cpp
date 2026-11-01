// 1.13c D2Game+0x34C00: quest-flags packet 0x28 (103 bytes). EAX = unit, stack = pGame (ret 4). The player data (unit passed even when null) holds
// the quest-flag pointer at +0x10+4*game[0x6D]; the packet is {0x28, 6, 0 x5, then 0x60 bytes exported by 0xA43C (flags, buffer, 0x60, 0)};
// the client word is playerData+0x9C for a player unit (type 0) else 0. Seam: r[0]=unit, r[1]=pGame, r[5]=env.
#include <cstdint>
#include <cstring>
#include "d2game_batch20env.h"

extern "C" void D2Game_34c00(uint32_t* r) {
    const D2GameBatch20Env& env = *(const D2GameBatch20Env*)(uintptr_t)r[5];
    void* unit = (void*)(uintptr_t)r[0];
    uint32_t pd = (uint32_t)(uintptr_t)env.getPlayerData(unit), flags, client = 0;
    uint8_t act; std::memcpy(&act, (const void*)(uintptr_t)(r[1] + 0x6D), 1);
    std::memcpy(&flags, (const void*)(uintptr_t)(pd + 0x10 + 4u * act), 4);
    if (unit) { uint32_t t; std::memcpy(&t, unit, 4); if (t == 0) { uint32_t pd2 = (uint32_t)(uintptr_t)env.getPlayerData(unit); std::memcpy(&client, (const void*)(uintptr_t)(pd2 + 0x9C), 4); } }
    uint8_t packet[0x67] = {};
    packet[0] = 0x28; packet[1] = 6;
    env.exportQuestFlags((void*)(uintptr_t)flags, packet + 7, 0x60, 0);
    env.sendPacket(client, packet, 0x67);
    r[4] = 0;
}
