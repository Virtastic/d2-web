// 1.13c D2Game+0x35B60: send the 0x91 NPC-intro packet of one stage. EAX = unit, BL = stage, stack: pGame (ret 4).
// The player data is fetched once up front (its intro record = playerData[0x60 + game[0x6D]*4]); stage 0/1/2/4 pick the id list of
// 0x35C20, 0x34C70 fills the packet and says whether any id was appended; if so the 26-byte packet {0x91, stage, 12 x u16} goes
// out through 0x8A3E0 with EAX = the client word playerData[0x9C] (0 unless the unit is a player).
// Port seam: r[0]=unit, r[1]=stage, r[2]=pGame, r[5]=D2GameBatch20Env.
#include <cstdint>
#include <cstring>
#include "d2game_batch20env.h"

extern "C" void D2Game_34c70(uint32_t* r);

namespace {
const uint32_t kStage0[6]  = { 148, 154, 150, 147, 155, 265 };
const uint32_t kStage1[11] = { 175, 176, 177, 178, 202, 200, 210, 201, 198, 199, 244 };
const uint32_t kStage2[7]  = { 245, 252, 253, 254, 255, 264, 297 };
const uint32_t kStage4[7]  = { 520, 521, 511, 512, 513, 514, 515 };
inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
}

extern "C" void D2Game_35b60(uint32_t* r) {
    const D2GameBatch20Env& env = *(const D2GameBatch20Env*)(uintptr_t)r[5];
    void* unit = (void*)(uintptr_t)r[0];
    const uint8_t stage = (uint8_t)r[1];
    const uint8_t* game = (const uint8_t*)(uintptr_t)r[2];
    void* pd = env.getPlayerData(unit);
    const uint32_t intro = rd32(pd, 0x60 + (uint32_t)game[0x6D] * 4);
    const uint32_t* ids; uint32_t n;
    switch (stage) {
    case 0: ids = kStage0; n = 6; break;
    case 1: ids = kStage1; n = 11; break;
    case 2: ids = kStage2; n = 7; break;
    case 4: ids = kStage4; n = 7; break;
    default: r[4] = 0; return;
    }
    uint8_t packet[26];
    std::memset(packet, 0, sizeof packet);
    uint32_t q[8] = { intro, n, (uint32_t)(uintptr_t)ids, (uint32_t)(uintptr_t)packet, 0, r[5] };
    D2Game_34c70(q);
    if (q[4] == 0) { r[4] = 0; return; }
    packet[0] = 0x91; packet[1] = stage;
    uint32_t client = 0;
    if (unit != nullptr && rd32(unit, 0) == 0) client = rd32(env.getPlayerData(unit), 0x9C);
    env.sendPacket(client, packet, 0x1A);
    r[4] = 0;
}
