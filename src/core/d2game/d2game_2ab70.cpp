// 1.13c D2Game+0x2AB70 (54 B, ESI = client id): the game of the current thread (Fog 10015 gives its guid, 0x2AAE0 finds and locks it) if
// the client is on its client list (game+0x88, next through +0x4A8, id at +0); otherwise the game lock is released (0x29BA0) and the result is null.
#include "d2game_gamelist.h"

struct D2G2AB70Env {
    uint32_t (*currentGuid)(uint32_t nClientId);   // Fog ordinal 10015 (0xA364), stdcall 1: the guid of the game the client is in
    uint8_t* (*lockGame)(uint32_t);      // 0x2AAE0
    void (*unlockGame)(uint8_t*);        // 0x29BA0, EAX = game
};

// 1.13c D2Game+0x2AB70
uint8_t* D2Game_2AB70(uint32_t nClientId, const D2G2AB70Env* pEnv)
{
    uint8_t* pGame = pEnv->lockGame(pEnv->currentGuid(nClientId));
    if (pGame == nullptr) return nullptr;
    for (uint8_t* p = (uint8_t*)d2gl::rdptr(pGame, 0x88); p != nullptr; p = (uint8_t*)d2gl::rdptr(p, 0x4A8))
        if (d2gl::rd32(p, 0) == nClientId) return pGame;
    pEnv->unlockGame(pGame);
    return nullptr;
}
