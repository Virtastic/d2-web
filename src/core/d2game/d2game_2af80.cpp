// 1.13c D2Game+0x2AF80 (stdcall ret 4: stats out-struct): counts the running games and their units. The out words are cleared, then the slot table
// is walked from index 1 for 1024 slots; a guid other than 0 and -1 is found and locked by 0x2AAE0 (null: skipped). For each of the 128 room lists
// the chain in game+0x1320+4*i (linked through +0xE4) adds to word 1 and the chain in game+0x1120+4*i adds to word 3; a chain pointer that is
// non-null never reaches the 0xDF assert. The game lock is left (null is the 0x100F assert) and word 0 counts the game.
#include "d2game_gamelist.h"

struct D2G2AF80Env {
    uint8_t* (*lockGame)(uint32_t nGuid);   // 0x2AAE0
    void (*leaveGame)(void* pLock);
};

// 1.13c D2Game+0x2AF80
void D2Game_2AF80(uint32_t* pStats, const D2G2AF80Env* pEnv)
{
    pStats[0] = pStats[1] = pStats[2] = pStats[3] = 0;
    for (uint32_t i = 1; i <= 0x400; ++i)
    {
        const uint32_t nGuid = d2gl::slotAt(i);
        if (nGuid == 0 || nGuid == 0xFFFFFFFFu) continue;
        uint8_t* pGame = pEnv->lockGame(nGuid);
        if (pGame == nullptr) continue;
        for (uint32_t n = 0; n < 0x80; ++n)
        {
            for (void* p = d2gl::rdptr(pGame, 0x1320 + 4 * n); p != nullptr; p = d2gl::rdptr(p, 0xE4)) ++pStats[1];
            for (void* p = d2gl::rdptr(pGame, 0x1120 + 4 * n); p != nullptr; p = d2gl::rdptr(p, 0xE4)) ++pStats[3];
        }
        void* pLock = d2gl::rdptr(pGame, 0x18);
        if (pLock == nullptr) d2gl::fatal(0x100F);
        pEnv->leaveGame(pLock);
        ++pStats[0];
    }
}
