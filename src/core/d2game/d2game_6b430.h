// Seam of D2Game+0x6B430 (rotating code-item spawn). pCtx = {+0 game, +8 unit}; the callees are hooked functions in the capture.
#pragma once
#include <cstdint>
struct D2GameCodeItemEnv {
    uint32_t (*nextItem)(uint32_t pList, uint32_t nKind, uint32_t pCur);       // 0xB036 (3 stack words: list, 0x14, current)
    uint32_t (*itemRec)(uint32_t nId);                                         // 0xA53E (1 stack word)
    int32_t (*codeIdx)(uint32_t nCode);                                        // 0xAE74 (1 stack word)
    uint32_t (*place)(uint32_t pGame, uint32_t pUnit, uint32_t nIdx, uint32_t pItem);   // 0x6B070 (4 stack words)
    void (*fatal)(uint32_t nLine);
};
void D2Game_6B430(uint32_t pCtx, const D2GameCodeItemEnv* pEnv);
