// 1.13c D2Game+0x2B840 (185 B, stdcall ret 8): game-age histogram. pHist[nBins] words are zeroed, then for every live game
// (slots[1..0x400], not 0/-1, locked through 0x2AAE0, a null game skipped) the age bin is (frame / 25 seconds) / 300 -- 5-minute bins --
// clamped to nBins - 1, and pHist[bin] is incremented (a word). The frame is the game's +0xA8 counter, read as signed; the division is
// the compiler's multiply-high sequence (0x51EB851F >> 3, 0x1B4E81B5 >> 5, each plus its sign bit). The game lock (+0x18) is left before the
// increment (null -> the 0x100F assert).
#include "d2game_gamelist.h"

struct D2Game2B840Env { void* (*lockGame)(uint32_t nGameGuid); };   // 0x2AAE0

// 1.13c D2Game+0x2B840
void D2Game_2B840(uint16_t* pHist, int32_t nBins, const D2Game2B840Env* pEnv)
{
    std::memset(pHist, 0, 2u * static_cast<uint32_t>(nBins));
    for (uint32_t i = 1; i <= 0x400; ++i)
    {
        const uint32_t nGuid = d2gl::g_state.slots[i];
        if (nGuid == 0 || nGuid == 0xFFFFFFFFu) continue;
        void* pGame = pEnv->lockGame(nGuid);
        if (pGame == nullptr) continue;

        const int32_t nFrame = static_cast<int32_t>(d2gl::rd32(pGame, 0xA8));
        int32_t t = static_cast<int32_t>((static_cast<int64_t>(nFrame) * 0x51EB851F) >> 32) >> 3;
        const int32_t nSeconds = t + static_cast<int32_t>(static_cast<uint32_t>(t) >> 31);
        d2gl::leaveGameLock(pGame);
        int32_t m = static_cast<int32_t>((static_cast<int64_t>(nSeconds) * 0x1B4E81B5) >> 32) >> 5;
        int32_t nBin = m + static_cast<int32_t>(static_cast<uint32_t>(m) >> 31);
        if (nBin >= nBins) nBin = nBins - 1;
        ++pHist[nBin];
    }
}
