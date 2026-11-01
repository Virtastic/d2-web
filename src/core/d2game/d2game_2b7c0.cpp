// 1.13c D2Game+0x2B7C0 (ord 10045, 126 B, stdcall ret 4): the client count (+0x8C) of the game with id nGameId (a word on the stack).
// Fog #10050 enters the game-list lock, GUID = slots[nGameId] (0 and -1 -> 0), 0x2AAE0 locks the game; no game is the 0x1273
// assert, a null game lock the 0x100F assert.
#include "d2game_gamelist.h"

struct D2Game2B7C0Env {
    void (*enterGameList)();                 // Fog #10050 (CS 0x6FD30BE8, line 0x125)
    void* (*lockGame)(uint32_t nGameGuid);   // 0x2AAE0, GUID pushed (ret 4)
};

// 1.13c D2Game+0x2B7C0
uint32_t D2Game_2B7C0(uint16_t nGameId, const D2Game2B7C0Env* pEnv)
{
    pEnv->enterGameList();
    LeaveCriticalSection(&d2gl::g_listLock);
    const uint32_t nGuid = d2gl::usableGuid(d2gl::slotAt(nGameId));
    void* pGame = pEnv->lockGame(nGuid);
    if (pGame == nullptr) d2gl::fatal(0x1273);
    const uint32_t nClients = d2gl::rd32(pGame, d2gl::kGameClients);
    d2gl::leaveGameLock(pGame);
    return nClients;
}
