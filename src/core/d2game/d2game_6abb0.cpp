// 1.13c D2Game+0x6ABB0: spawn the effect row's missile. Register convention: ECX = ctx, EDX = request record, no result.
// idx = byte rec[0] must be below the row count (module +0x107D4C), else Fog fatal line 0x9F9. Row = table[idx] = {-, sel, arg}. v = (sel == -1) ? 0 :
// 0x69EE0(ESI = ctx[8], EBX = rec[4], sel); then 0x6A960(ECX = ctx[4], EDX = ctx[8], rec[0xC], v, sel, arg, 0).
#include <cstdint>
#include <cstring>
#include "d2game_6abb0.h"

static inline uint32_t rd32(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }

// 1.13c D2Game+0x6ABB0
void D2Game_6ABB0(const uint8_t* pCtx, const uint8_t* pRec, const uint32_t* pTable, int32_t nCount, const D2GameSpawnEnv* pEnv)
{
    const int32_t nIdx = pRec[0];
    if (nIdx >= nCount) { pEnv->fatal(0x9F9); return; }
    const uint32_t* pRow = pTable + 3 * nIdx;
    const uint32_t nSel = pRow[1];
    const uint32_t v = nSel != 0xFFFFFFFFu ? pEnv->statValue(rd32(pCtx, 8), rd32(pRec, 4), nSel) : 0;
    pEnv->createMissile(rd32(pCtx, 4), rd32(pCtx, 8), rd32(pRec, 0xC), v, nSel, pRow[2], 0);
}
