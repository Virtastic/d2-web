// 1.13c D2Game+0x2B9A0 (103 B, cdecl): how many live games have the expansion flag (+0x70) set. Walks slots[1..0x400] (not 0/-1),
// locks each game through 0x2AAE0 (a null game is skipped), counts a non-zero dword at +0x70 and leaves the game's lock (null -> 0x100F).
#include "d2game_gamelist.h"

struct D2Game2B9A0Env { void* (*lockGame)(uint32_t nGameGuid); };   // 0x2AAE0

// 1.13c D2Game+0x2B9A0
uint32_t D2Game_2B9A0(const D2Game2B9A0Env* pEnv)
{
    uint32_t n = 0;
    for (uint32_t i = 1; i <= 0x400; ++i)
    {
        const uint32_t nGuid = d2gl::g_state.slots[i];
        if (nGuid == 0 || nGuid == 0xFFFFFFFFu) continue;
        void* pGame = pEnv->lockGame(nGuid);
        if (pGame == nullptr) continue;
        if (d2gl::rd32(pGame, 0x70) != 0) ++n;
        d2gl::leaveGameLock(pGame);
    }
    return n;
}
