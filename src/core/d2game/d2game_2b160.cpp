// 1.13c D2Game+0x2B160 (455 B, stdcall ret 0xC): counts the units of one type in a game, per unit id, then reports each id. The game comes from the
// game-list slot nId (a word; 0 and -1 are not games) through 0x2AAE0; none returns at once. Type nType (0..4) selects the bucket array game+0x1120 +
// 0x200 * nType (128 dwords, chains through unit+0xE4). Each unit's id (unit+4) is looked up in the list that starts at pHead (a node is 0x90 bytes:
// +0 id, +4 count, +0x8C next); found = count + 1, else a node is allocated (Fog #10045, Game.cpp line 0x135C), zeroed, filled (id, count 1) and linked
// right after pHead. A null pHead with a unit is the 0x134B assert. Then the game lock (+0x18; null = 0x100F) is left, IsBadCodePtr(table[nType],
// D2Game+0x10E3D0) is the 0x13FD assert, and table[nType] runs for every node after pHead with EDX = id and ECX = node + 0xC.
#include "d2game_gamelist.h"

struct D2Game2B160Env {
    void* (*lockGame)(uint32_t nGameGuid);                            // 0x2AAE0
    void* (*alloc)();                                                  // Fog #10045 (0xA2E6, 0x90 bytes, line 0x135C)
    uint32_t (*isBadHandler)(uint32_t nType);                         // IsBadCodePtr(table[nType])
    void (*callHandler)(uint32_t nType, uint32_t nId, uint32_t nNode0C);   // table[nType] (EDX, ECX)
    void (*fatal)(uint32_t nLine);
    void (*enterList)();                                               // Fog #10050 (0xA328)
};

static inline uint8_t* P(uint32_t n) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(n)); }
static inline uint32_t H(const void* p) { return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p)); }

// 1.13c D2Game+0x2B160
void D2Game_2B160(uint32_t nId, uint32_t nHead, uint32_t nType, const D2Game2B160Env* pEnv)
{
    pEnv->enterList();
    const uint32_t nGuid = d2gl::usableGuid(d2gl::slotAt(nId & 0xFFFF));
    void* pGame = pEnv->lockGame(nGuid);
    if (pGame == nullptr) return;
    if (nType > 4) return;   // retail indexes a null bucket base here (a crash); not modelled
    const uint32_t nBase = 0x1120 + 0x200 * nType;
    for (uint32_t b = 0; b < 0x80; ++b)
    {
        for (uint32_t nUnit = d2gl::rd32(pGame, nBase + 4 * b); nUnit != 0; nUnit = d2gl::rd32(P(nUnit), 0xE4))
        {
            const uint32_t nUnitId = d2gl::rd32(P(nUnit), 4);
            if (nHead == 0) { pEnv->fatal(0x134B); return; }
            uint32_t nNode = nHead;
            bool bFound = false;
            for (; nNode != 0; nNode = d2gl::rd32(P(nNode), 0x8C))
                if (d2gl::rd32(P(nNode), 0) == nUnitId) { const uint32_t c = d2gl::rd32(P(nNode), 4) + 1; std::memcpy(P(nNode) + 4, &c, 4); bFound = true; break; }
            if (bFound) continue;
            uint8_t* pNew = static_cast<uint8_t*>(pEnv->alloc());
            if (pNew == nullptr) continue;
            std::memset(pNew, 0, 0x90);
            const uint32_t one = 1, next = d2gl::rd32(P(nHead), 0x8C), me = H(pNew);
            std::memcpy(pNew, &nUnitId, 4); std::memcpy(pNew + 4, &one, 4); std::memcpy(pNew + 0x8C, &next, 4);
            std::memcpy(P(nHead) + 0x8C, &me, 4);
        }
    }
    d2gl::leaveGameLock(pGame);
    if (pEnv->isBadHandler(nType)) { pEnv->fatal(0x13FD); return; }
    for (uint32_t nNode = d2gl::rd32(P(nHead), 0x8C); nNode != 0; nNode = d2gl::rd32(P(nNode), 0x8C))
        pEnv->callHandler(nType, d2gl::rd32(P(nNode), 0), nNode + 0xC);
}
