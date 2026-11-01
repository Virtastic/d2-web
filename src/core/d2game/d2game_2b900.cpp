// 1.13c D2Game+0x2B900 (145 B, stdcall ret 8): client-count histogram. pHist[nMax + 1] words are zeroed, then for every
// live game (slots[1..0x400], not 0/-1, locked through 0x2AAE0, a null game skipped) the client count (+0x8C) is read, the game's lock
// (+0x18) left (null -> the 0x100F assert), the count clamped to nMax (signed) and pHist[count] incremented (a word).
#include "d2game_gamelist.h"

struct D2Game2B900Env { void* (*lockGame)(uint32_t nGameGuid); };   // 0x2AAE0

// 1.13c D2Game+0x2B900
void D2Game_2B900(uint16_t* pHist, int32_t nMax, const D2Game2B900Env* pEnv)
{
    std::memset(pHist, 0, 2u * static_cast<uint32_t>(nMax) + 2u);
    for (uint32_t i = 1; i <= 0x400; ++i)
    {
        const uint32_t nGuid = d2gl::g_state.slots[i];
        if (nGuid == 0 || nGuid == 0xFFFFFFFFu) continue;
        void* pGame = pEnv->lockGame(nGuid);
        if (pGame == nullptr) continue;

        int32_t nClients = static_cast<int32_t>(d2gl::rd32(pGame, d2gl::kGameClients));
        d2gl::leaveGameLock(pGame);
        if (nClients > nMax) nClients = nMax;
        ++pHist[nClients];
    }
}
