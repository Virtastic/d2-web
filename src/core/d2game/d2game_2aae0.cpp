// 1.13c D2Game+0x2AAE0 (stdcall, game guid): finds a game by guid under the game table's critical section (table+0x50)
// and hands it back with the game's own lock (+0x18) taken; the table lock is left again before returning. A null table or an
// unknown guid gives null; a game whose lock pointer is null is the 0x112E assert.
#include "d2game_gamelist.h"

struct D2G2AAE0Env {
    void (*enterTable)(void* pCs);                            // EnterCriticalSection(table + 0x50), imported
    void (*leaveTable)(void* pCs);                            // LeaveCriticalSection, imported
    uint8_t* (*findGame)(uint8_t* pTable, uint32_t nGuid);    // 0x2130: EDX = table, ESI = guid
    void (*lockGame)(void* pLock, uint32_t nLine);            // 0xA328: ECX = game+0x18, EDX = line 0x1141
};

// 1.13c D2Game+0x2AAE0
uint8_t* D2Game_2AAE0(uint8_t* pTable, uint32_t nGuid, const D2G2AAE0Env* pEnv)
{
    if (pTable == nullptr) return nullptr;
    void* pCs = pTable + 0x50;
    pEnv->enterTable(pCs);
    uint8_t* pGame = pEnv->findGame(pTable, nGuid);
    if (pGame == nullptr)
    {
        pEnv->leaveTable(pCs);
        return nullptr;
    }
    if (d2gl::rd32(pGame, 0x18) == 0) d2gl::fatal(0x112E);
    pEnv->lockGame(d2gl::rdptr(pGame, 0x18), 0x1141);
    pEnv->leaveTable(pCs);
    return pGame;
}
