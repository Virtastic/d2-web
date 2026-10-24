// 1.13c D2Game+0x5A000: allocate a stat list owned by the unit, post it
// (reset 1), set the four resists (0x27 fire, 0x29 light, 0x2B cold, 0x2D poison) to nValue, then push each to the client.
#include "game_5a000.h"

void GAME_5A000(uint32_t pUnit, uint32_t nValue, const uint32_t* pGame, D2G5A000AllocFn alloc, D2G5A000PostFn post,
                D2G5A000SetFn set, D2G5A000UpdFn upd)
{
    const uint32_t* u = (const uint32_t*)(uintptr_t)pUnit;   // +0x00 type, +0x0C id
    const uint32_t nType = pUnit ? u[0] : 6, nId = pUnit ? u[3] : 0xFFFFFFFFu;
    const uint32_t list = alloc(pGame[7], 0, 0, nType, nId);   // pGame+0x1C = pMemoryPool
    if (!list) return;
    post(pUnit, list, 1);
    static const uint32_t stats[4] = { 0x27, 0x29, 0x2B, 0x2D };
    for (uint32_t s : stats) set(list, s, nValue, 0);
    for (uint32_t s : stats) upd(pUnit, s, nValue, pUnit);
}
