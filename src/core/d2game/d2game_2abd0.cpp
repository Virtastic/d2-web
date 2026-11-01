// 1.13c D2Game+0x2ABD0 (stdcall, name): looks for a running game by name. The 1024 slot guids (slot table from index 1) that are neither 0 nor -1
// go through 0x2AAE0; a found game's name (game+0x2A, 16 bytes) is compared with Storm 509 and a match leaves the game lock and returns 0, any other
// game leaves its lock (a null lock is the 0x100F assert) and the walk goes on. No match gives 1.
#include "d2game_gamelist.h"

struct D2G2ABD0Env {
    uint8_t* (*lockGame)(uint32_t nGuid);                            // 0x2AAE0
    int32_t (*strCmp)(const void* a, const void* b, uint32_t n);     // 0xA286, Storm 509
    void (*leaveCs)(void* pCs);                                      // LeaveCriticalSection
};

// 1.13c D2Game+0x2ABD0
uint32_t D2Game_2ABD0(const void* pName, const D2G2ABD0Env* pEnv)
{
    for (uint32_t i = 1; i <= 0x400; ++i)
    {
        const uint32_t nGuid = d2gl::slotAt(i);
        if (nGuid == 0 || nGuid == 0xFFFFFFFFu) continue;
        uint8_t* pGame = pEnv->lockGame(nGuid);
        if (pGame == nullptr) continue;
        const int32_t nCmp = pEnv->strCmp(pName, pGame + 0x2A, 0x10);
        void* pLock = d2gl::rdptr(pGame, 0x18);
        if (pLock == nullptr) d2gl::fatal(0x100F);
        pEnv->leaveCs(pLock);
        if (nCmp == 0) return 0;
    }
    return 1;
}
