// 1.13c D2Game+0x6AC30: spawn the effect row's missile and arm it. Register convention: ECX = ctx, EDX = request record, no result.
// idx = byte rec[0] must be below the row count, else Fog fatal line 0x9E7. u = ctx[8]; 0xA472(u, 0xA, 0xA460(u), 0); v = 0x69EE0(ESI = u, EBX = rec[4], row.sel);
// m = 0x6A960(ECX = ctx[4], EDX = u, rec[0xC], v, row.sel, row.arg, callback module+0x6A900); then 0xA76C(m, 0xA, 2v, 0) and 0xA76C(m, 0x1C, 1000, 0).
#include <cstdint>
#include <cstring>
#include "d2game_6abb0.h"

static inline uint32_t rd32(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }

// 1.13c D2Game+0x6AC30
void D2Game_6AC30(const uint8_t* pCtx, const uint8_t* pRec, const uint32_t* pTable, int32_t nCount, const D2GameSpawnEnv* pEnv)
{
    const int32_t nIdx = pRec[0];
    if (nIdx >= nCount) { pEnv->fatal(0x9E7); return; }
    const uint32_t* pRow = pTable + 3 * nIdx;
    const uint32_t u = rd32(pCtx, 8);
    const uint32_t a = pEnv->unitA460(u);
    pEnv->unitA472(u, 0xA, a, 0);
    const uint32_t v = pEnv->statValue(u, rd32(pRec, 4), pRow[1]);
    const uint32_t m = pEnv->createMissile(rd32(pCtx, 4), u, rd32(pRec, 0xC), v, pRow[1], pRow[2], 0x6A900u);   // w4 is the module address of 0x6A900 (callback)
    pEnv->unitA76C(m, 0xA, v + v, 0);
    pEnv->unitA76C(m, 0x1C, 1000, 0);
}
