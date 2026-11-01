// 1.13c D2Game+0x34B80: quest-state packet 0x27 (40 bytes). EAX = unit, ECX = quest unit (its +0xC id, or -1 when null), stack word = u16 value
// (ret 4). {0x27, 2, id32 at +2, 1 at +6, 0 at +8, value16 at +10}; the other bytes of the retail buffer are stack residue (ours: zero).
// The send takes the client word playerData+0x9C when the unit is a player (type 0), else 0. Seam: r[0]=unit, r[1]=quest unit, r[2]=value, r[5]=env.
#include <cstdint>
#include <cstring>
#include "d2game_batch20env.h"

extern "C" void D2Game_34b80(uint32_t* r) {
    const D2GameBatch20Env& env = *(const D2GameBatch20Env*)(uintptr_t)r[5];
    void* unit = (void*)(uintptr_t)r[0];
    uint32_t client = 0, id = 0xFFFFFFFFu;
    if (unit) { uint32_t t; std::memcpy(&t, unit, 4); if (t == 0) { uint32_t pd = (uint32_t)(uintptr_t)env.getPlayerData(unit); std::memcpy(&client, (const void*)(uintptr_t)(pd + 0x9C), 4); } }
    if (r[1]) std::memcpy(&id, (const void*)(uintptr_t)(r[1] + 0xC), 4);
    uint8_t packet[40] = {};
    packet[0] = 0x27; packet[1] = 2; std::memcpy(packet + 2, &id, 4); packet[6] = 1; packet[8] = 0;
    uint16_t v = (uint16_t)r[2]; std::memcpy(packet + 10, &v, 2);
    env.sendPacket(client, packet, 0x28);
    r[4] = 0;
}
