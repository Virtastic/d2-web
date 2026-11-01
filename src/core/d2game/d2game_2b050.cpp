// 1.13c D2Game+0x2B050 (270 B, stdcall ret 0x14): looks up nCount (kind, id) pairs in one game and fills an entry per unit found. The game comes
// from the game-list slot nId (a word; 0 and -1 are not games) through 0x2AAE0; none = 0. For each index i the unit is 0xE03A0
// (EAX = pKinds[i], EDX = pIds[i], ECX = game); a found unit gets its entry written by 0xDFB30 (ESI = unit, EDI = pBuf + 0x34 * found), and a
// non-zero kind then calls the handler table[kind] (D2Game+0x10E3D0, checked with IsBadCodePtr: bad = the 0x141F assert) with EDX = unit+4
// and ECX = entry + 0x24. The result is the number of units found; a null game lock (+0x18) at the end is the 0x100F assert.
#include "d2game_gamelist.h"

struct D2Game2B050Env {
    void* (*lockGame)(uint32_t nGameGuid);                            // 0x2AAE0
    void* (*findUnit)(uint32_t nKind, uint32_t nId, void* pGame);     // 0xE03A0 (plain ret)
    void (*writeEntry)(void* pUnit, uint32_t nEntry);                 // 0xDFB30
    uint32_t (*isBadHandler)(uint32_t nKind);                         // IsBadCodePtr(table[kind])
    void (*callHandler)(uint32_t nKind, uint32_t nUnitDword4, uint32_t nEntry24);   // table[kind] (EDX, ECX)
    void (*fatal)(uint32_t nLine);
    void (*enterList)();                                              // Fog #10050 (0xA328)
};

// 1.13c D2Game+0x2B050
int32_t D2Game_2B050(uint32_t nId, int32_t nCount, uint32_t nBuf, const uint8_t* pKinds, const uint8_t* pIds, const D2Game2B050Env* pEnv)
{
    pEnv->enterList();
    const uint32_t nGuid = d2gl::usableGuid(d2gl::slotAt(nId & 0xFFFF));
    void* pGame = pEnv->lockGame(nGuid);
    if (pGame == nullptr) return 0;
    int32_t nFound = 0;
    uint32_t nEntry = nBuf;
    for (int32_t i = 0; i < nCount; ++i)
    {
        const uint32_t nKind = d2gl::rd32(pKinds, 4 * i);
        void* pUnit = pEnv->findUnit(nKind, d2gl::rd32(pIds, 4 * i), pGame);
        if (pUnit == nullptr) continue;
        pEnv->writeEntry(pUnit, nEntry);
        if (nKind != 0)
        {
            if (pEnv->isBadHandler(nKind)) { pEnv->fatal(0x141F); return nFound; }
            pEnv->callHandler(nKind, d2gl::rd32(pUnit, 4), nEntry + 0x24);
        }
        ++nFound;
        nEntry += 0x34;
    }
    d2gl::leaveGameLock(pGame);
    return nFound;
}
