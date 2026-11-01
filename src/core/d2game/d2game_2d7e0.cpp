// 1.13c D2Game+0x2D7E0 (EAX = client id, [esp+4] = nArg, ret 4): looks the id up in the locked game (0x2AB70, ESI = id), finds the client in
// its list (0x49FE0, EAX = game, ECX = id), calls 0x2D090 (ESI = client; [esp+4] = game, [esp+8] = arg) and leaves the game lock (0x29BA0). Id 0 or no game: nothing.
#include <cstdint>

struct D2Game2D7E0Env {
    void* (*lockGameOfClient)(uint32_t nId);                      // 0x2AB70
    void* (*getClient)(void* pGame, uint32_t nId);                // 0x49FE0
    void (*notify)(void* pClient, void* pGame, uint32_t nArg);    // 0x2D090
    void (*leaveGame)(void* pGame);                               // 0x29BA0
};

// 1.13c D2Game+0x2D7E0
void D2Game_2D7E0(uint32_t nClientId, uint32_t nArg, const D2Game2D7E0Env* pEnv)
{
    if (nClientId == 0) return;
    void* pGame = pEnv->lockGameOfClient(nClientId);
    if (pGame == nullptr) return;
    void* pClient = pEnv->getClient(pGame, nClientId);
    pEnv->notify(pClient, pGame, nArg);
    pEnv->leaveGame(pGame);
}
