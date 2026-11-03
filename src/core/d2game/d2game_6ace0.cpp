// 1.13c D2Game+0x6ACE0: spawn the effect row's missile with selector 0x13. Register convention: ECX = ctx, EDX = request record, no result.
// idx = byte rec[0] must be below the row count, else Fog fatal line 0x9C1. v = 0x69EE0(ESI = ctx[8], EBX = rec[4], 0x13);
// m = 0x6A960(ECX = ctx[4], EDX = ctx[8], rec[0xC], rec[8], 0x19, row.arg, 0); then 0xA76C(m, 0x13, v, 0).
#include <cstdint>
#include <cstring>
#include "d2game_6abb0.h"

static inline uint32_t rd32(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }

// 1.13c D2Game+0x6ACE0
void D2Game_6ACE0(const uint8_t* pCtx, const uint8_t* pRec, const uint32_t* pTable, int32_t nCount, const D2GameSpawnEnv* pEnv)
{
    const int32_t nIdx = pRec[0];
    if (nIdx >= nCount) { pEnv->fatal(0x9C1); return; }
    const uint32_t* pRow = pTable + 3 * nIdx;
    const uint32_t v = pEnv->statValue(rd32(pCtx, 8), rd32(pRec, 4), 0x13);
    const uint32_t m = pEnv->createMissile(rd32(pCtx, 4), rd32(pCtx, 8), rd32(pRec, 0xC), rd32(pRec, 8), 0x19, pRow[2], 0);
    pEnv->unitA76C(m, 0x13, v, 0);
}
