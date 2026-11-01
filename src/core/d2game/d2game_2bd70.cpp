// 1.13c D2Game+0x2BD70 (ord 10055, 115 B, cdecl): the init seed (+0x7C) of the first game in the GUID table (slots[1]).
// Fog #10050 enters the game-list lock; 0x2AAE0 locks the game and the result is stored in slots[0] (0x6FD30C04) whether or not a
// game came back. No game yields 0 (the game lock is not left, there is none); a null game lock is the 0x100F assert.
#include "d2game_gamelist.h"

struct D2Game2BD70Env {
    void (*enterGameList)();                 // Fog #10050
    void* (*lockGame)(uint32_t nGameGuid);   // 0x2AAE0
};

// 1.13c D2Game+0x2BD70
uint32_t D2Game_2BD70(const D2Game2BD70Env* pEnv)
{
    pEnv->enterGameList();
    LeaveCriticalSection(&d2gl::g_listLock);
    void* pGame = pEnv->lockGame(d2gl::usableGuid(d2gl::g_state.slots[1]));
    d2gl::g_state.slots[0] = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pGame));
    if (pGame == nullptr) return 0;
    const uint32_t nSeed = d2gl::rd32(pGame, d2gl::kGameInitSeed);
    d2gl::leaveGameLock(pGame);
    return nSeed;
}
