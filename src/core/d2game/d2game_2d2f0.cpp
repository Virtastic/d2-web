// 1.13c D2Game+0x2D2F0 (180 B, stdcall ret 0xC): drops the client with id nId from the current game and reports through Fog #10001. The id goes
// through Fog #10015, the game is looked up and locked by 0x2AAE0 (stack = that value; null = skip to the report). The game's client list
// (game+0x88, next at +0x4A8, id at +0) is searched: no match = 0x29BA0 (EAX = game) and the report; a match = 0x4B7F0 (EAX = id, stack game, 1)
// and the game lock (+0x18; null = 0x100F assert) is left. The report is Fog #10001 (nId, a, b) in every path.
#include "d2game_gamelist.h"

struct D2Game2D2F0Env {
    uint32_t (*fog10015)(uint32_t nId);
    void* (*lockGame)(uint32_t nValue);                                // 0x2AAE0
    void (*unlockGame)(void* pGame);                                   // 0x29BA0
    void (*dropClient)(uint32_t nId, void* pGame, uint32_t nFlag);     // 0x4B7F0
    void (*leaveCs)(void* pCs);
    void (*fog10001)(uint32_t nId, uint32_t nA, uint32_t nB);
    void (*fatal)(uint32_t nLine);
};

static bool d2game_2d2f0_has(const void* pGame, uint32_t nId)
{
    for (const void* p = d2gl::rdptr(pGame, 0x88); p != nullptr; p = d2gl::rdptr(p, 0x4A8))
        if (d2gl::rd32(p, 0) == nId) return true;
    return false;
}

// 1.13c D2Game+0x2D2F0
void D2Game_2D2F0(uint32_t nId, uint32_t nA, uint32_t nB, const D2Game2D2F0Env* pEnv)
{
    void* pGame = pEnv->lockGame(pEnv->fog10015(nId));
    if (pGame != nullptr) {
        if (!d2game_2d2f0_has(pGame, nId)) {
            pEnv->unlockGame(pGame);
            pEnv->fog10001(nId, nA, nB);
            return;
        }
        pEnv->dropClient(nId, pGame, 1);
        void* pCs = d2gl::rdptr(pGame, 0x18);
        if (pCs == nullptr) { pEnv->fatal(0x100F); return; }
        pEnv->leaveCs(pCs);
    }
    pEnv->fog10001(nId, nA, nB);
}
