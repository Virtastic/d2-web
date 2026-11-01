// 1.13c D2Game+0x2B350 (226 B, stdcall ret 0xC): copies up to nCount unit entries out of one game. The game comes from the game-list
// slot nId (a word; 0 and -1 are not games) through 0x2AAE0. Its 128 unit-list buckets (game+0x1120, one dword each) are walked in order,
// each bucket's chain through unit+0xE4; for each unit the entry writer 0xDFB30 (ESI = unit, EDI = pBuf + 0x34 * index) runs until nCount
// entries exist. A null game returns 0 at once. A null game lock (+0x18) at the end is the 0x100F assert; the entry loop hitting a null
// unit is the 0xDF assert (unreachable here: the chain test precedes it). The result is the number of entries written.
#include "d2game_gamelist.h"

struct D2Game2B350Env {
    void* (*lockGame)(uint32_t nGameGuid);                 // 0x2AAE0
    void (*writeEntry)(uint32_t nIndex, void* pUnit, void* pEntry);   // 0xDFB30
    void (*fatal)(uint32_t nLine);
    void (*enterList)();                                   // Fog #10050: the game-list lock (0xA328, ECX = 0x110BE8, EDX = 0x125)
};

// 1.13c D2Game+0x2B350
int32_t D2Game_2B350(uint32_t nId, uint8_t* pBuf, int32_t nCount, const D2Game2B350Env* pEnv)
{
    pEnv->enterList();
    const uint32_t nGuid = d2gl::usableGuid(d2gl::slotAt(nId & 0xFFFF));
    void* pGame = pEnv->lockGame(nGuid);
    if (pGame == nullptr) return 0;
    int32_t n = 0;
    for (uint32_t b = 0; b < 0x80 && n < nCount; ++b)
    {
        uint32_t pUnit = d2gl::rd32(pGame, d2gl::kGameUnitList + 4 * b);
        while (pUnit != 0 && n < nCount)
        {
            pEnv->writeEntry(static_cast<uint32_t>(n), reinterpret_cast<void*>(static_cast<uintptr_t>(pUnit)), pBuf + 0x34u * static_cast<uint32_t>(n));
            ++n;
            pUnit = d2gl::rd32(reinterpret_cast<void*>(static_cast<uintptr_t>(pUnit)), 0xE4);
        }
    }
    d2gl::leaveGameLock(pGame);
    return n;
}
