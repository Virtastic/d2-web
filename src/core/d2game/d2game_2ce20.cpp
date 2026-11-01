// 1.13c D2Game+0x2CE20 (221 B, EAX = game GUID, plain ret): closes a game by GUID. No game table (0x6FD31C24) = nothing. Otherwise the
// table's lock (+0x50) is entered, the game found through 0x2130 (ESI = GUID, EDX = table) and, when the optional callback block
// (0x6FD31C3C) holds a function, that function is checked (IsBadCodePtr: bad = 0x1027 assert) and called fastcall with ECX = game id
// (u16 +0x28), EDX = 'D2DV' (+0x13FA when game+0x70 is set), stack = game+0x90 and game+0xA8 / 25. The game's own lock (+0x18)
// must exist (0x1038 assert), 0xA328 asserts it (EDX = 0x1039), then 0x2CD30 (EAX = game, ECX = GUID) frees the game and 0x1F30
// (EAX = table, EDI = game, [esp+4] = -1) removes it from the table.
#include <cstdint>
#include <cstring>

struct D2Game2CE20Env {
    void (*enterCs)(void* pCs);                                                     // EnterCriticalSection
    void (*leaveCs)(void* pCs);                                                     // LeaveCriticalSection
    void* (*findGame)(uint32_t nGuid, void* pTable);                                // 0x2130 (ESI = GUID, EDX = table)
    uint32_t (*isBadCodePtr)(uint32_t pFn);                                         // IsBadCodePtr
    void (*callback)(uint32_t pFn, uint32_t nId, uint32_t nTag, uint32_t nLast, int32_t nSeconds);   // fastcall ECX, EDX + 2 stack
    void (*assertLock)(uint32_t nLock, uint32_t nLine);                                // 0xA328 (ECX = lock, EDX = line)
    void (*freeGame)(void* pGame, uint32_t nGuid);                                  // 0x2CD30 (EAX = game, ECX = GUID)
    void (*removeGame)(void* pTable, void* pGame, int32_t nFlag);                   // 0x1F30 (EAX = table, EDI = game, [esp+4])
    void (*fatal)(uint32_t nLine);
};

// 1.13c D2Game+0x2CE20; pCallbackBlock is the block 0x6FD31C3C points at (its first dword is the function), or null.
void D2Game_2CE20(uint32_t nGuid, void* pTable, const uint32_t* pCallbackBlock, const D2Game2CE20Env* pEnv)
{
    if (pTable == nullptr) return;
    void* pCs = static_cast<uint8_t*>(pTable) + 0x50;
    pEnv->enterCs(pCs);
    void* pGame = pEnv->findGame(nGuid, pTable);
    if (pGame == nullptr) { pEnv->leaveCs(pCs); return; }
    const uint8_t* g = static_cast<const uint8_t*>(pGame);
    if (pCallbackBlock != nullptr && *pCallbackBlock != 0) {
        if (pEnv->isBadCodePtr(*pCallbackBlock)) { pEnv->fatal(0x1027); return; }
        uint16_t nId; uint32_t nExp, nLast; int32_t nFrame;
        std::memcpy(&nId, g + 0x28, 2); std::memcpy(&nExp, g + 0x70, 4); std::memcpy(&nLast, g + 0x90, 4); std::memcpy(&nFrame, g + 0xA8, 4);
        pEnv->callback(*pCallbackBlock, nId, 0x44324456u + (nExp != 0 ? 0x13FAu : 0u), nLast, nFrame / 25);
    }
    uint32_t nLock; std::memcpy(&nLock, g + 0x18, 4);
    if (nLock == 0) { pEnv->fatal(0x1038); return; }
    pEnv->assertLock(nLock, 0x1039);
    pEnv->freeGame(pGame, nGuid);
    pEnv->removeGame(pTable, pGame, -1);
}
