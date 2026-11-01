// 1.13c D2Game+0x2B4D0 (ord 10060, 118 B, stdcall ret 8): write the id (word at +0x28) of every live game, up to nMax, into
// pOut[]; returns how many. A slot of 0 or -1 is skipped, so is a GUID 0x2AAE0 cannot lock. Each locked game is unlocked at
// once (null game lock -> 0x100F). Unlike the single-game accessors it takes no game-list lock itself.
#include "d2game_gamelist.h"

struct D2Game2B4D0Env { void* (*lockGame)(uint32_t nGameGuid); };   // 0x2AAE0

// 1.13c D2Game+0x2B4D0
int32_t D2Game_2B4D0(uint16_t* pOut, int32_t nMax, const D2Game2B4D0Env* pEnv)
{
    int32_t n = 0;
    for (uint32_t i = 1; i <= 0x400; ++i)
    {
        if (n >= nMax) break;
        const uint32_t nGuid = d2gl::g_state.slots[i];
        if (nGuid == 0 || nGuid == 0xFFFFFFFFu) continue;
        void* pGame = pEnv->lockGame(nGuid);
        if (pGame == nullptr) continue;
        pOut[n++] = d2gl::rd16(pGame, d2gl::kGameId);
        d2gl::leaveGameLock(pGame);
    }
    return n;
}
