// 1.13c D2Game+0x2B440 (ord 10053, 132 B, stdcall ret 4): the unit count of the game with id nGameId (a word on the stack).
// Fog #10050 enters the game-list lock, GUID = slots[nGameId] (0 and -1 -> 0), 0x2AAE0 locks the game; no game yields 0.
// With a game, 0x2A680 (ESI = game + 0x1120, the 128-bucket unit hash) counts the units; then the game's lock (+0x18) is left (null -> 0x100F).
#include "d2game_gamelist.h"

struct D2Game2B440Env {
    void (*enterGameList)();                 // Fog #10050
    void* (*lockGame)(uint32_t nGameGuid);   // 0x2AAE0
    int32_t (*countGameUnits)(void* pGame);  // 0x2A680, ESI = pGame + 0x1120
};

// 1.13c D2Game+0x2B440
uint32_t D2Game_2B440(uint16_t nGameId, const D2Game2B440Env* pEnv)
{
    pEnv->enterGameList();
    LeaveCriticalSection(&d2gl::g_listLock);
    void* pGame = pEnv->lockGame(d2gl::usableGuid(d2gl::slotAt(nGameId)));
    uint32_t nUnits = 0;
    if (pGame != nullptr)
    {
        nUnits = static_cast<uint32_t>(pEnv->countGameUnits(pGame));
        d2gl::leaveGameLock(pGame);
    }
    return nUnits;
}
