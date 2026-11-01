// 1.13c D2Game+0x2BF90 (82 B, register convention EAX = pGame, EDI = pClient): makes sure the game's per-slot object exists for the
// client's slot byte (+0x1AC) -- a zero entry at game+0xBC+slot*4 is filled by 0x9D6F0 (ESI = game, EBX = slot) -- then runs 0x9D610
// (ESI = client, [esp+4] = game) and marks the client (+4) as 2. A null client is the 0x1BF assert.
#include <cstdint>
#include <cstdlib>
#include <cstring>

struct D2Game2BF90Env {
    void (*initSlot)(void* pGame, uint32_t nSlot);        // 0x9D6F0 (ESI = game, EBX = slot, plain ret)
    void (*fn9D610)(void* pClient, void* pGame);          // 0x9D610 (ESI = client, [esp+4] = game, ret 4)
    void (*fatal)(uint32_t nLine);                        // Fog #10265/#10024 + _exit(-1)
};

// 1.13c D2Game+0x2BF90
void D2Game_2BF90(void* pGame, void* pClient, const D2Game2BF90Env* pEnv)
{
    if (pClient == nullptr) { pEnv->fatal(0x1BF); return; }
    uint8_t* const pC = static_cast<uint8_t*>(pClient);
    const uint32_t nSlot = pC[0x1AC];
    uint32_t nEntry;
    std::memcpy(&nEntry, static_cast<uint8_t*>(pGame) + 0xBC + nSlot * 4, 4);
    if (nEntry == 0) pEnv->initSlot(pGame, nSlot);
    pEnv->fn9D610(pClient, pGame);
    const uint32_t nTwo = 2;
    std::memcpy(pC + 4, &nTwo, 4);
}
