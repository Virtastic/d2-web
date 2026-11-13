// Stat-value dispatcher reached through a pointer table: the same code as the 0x27120 row at another address (a second copy the
// linker kept). ECX = stat id, EDX = selector, stack: one unused word and a pointer to a holder whose first dword is the unit (ret 8).
// Null holder / unit, negative stat id or one at or past the stat-table count (sgptDataTables+0xBD4): 0. Stat 0x13 -> 0x31EA0
// (thiscall, ECX = 0x13, stack = unit); else selector 1 -> 0x38C20, 2 -> 0x39430, otherwise 0x38B70, as (unit, stat id, layer 0).
#include <cstdint>

struct D2StatDispatchSeam50C30 {
    int32_t (*pfnAttackRating)(uint32_t nEcx, uint32_t pUnit);
    int32_t (*pfnStat38B70)(uint32_t pUnit, int32_t nStat, int32_t nLayer);
    int32_t (*pfnStat38C20)(uint32_t pUnit, int32_t nStat, int32_t nLayer);
    int32_t (*pfnStat39430)(uint32_t pUnit, int32_t nStat, int32_t nLayer);
    int32_t nTableCount;   // sgptDataTables + 0xBD4
};

// 1.13c D2Common+0x50C30
extern "C" int32_t D2Common_50C30_StatDispatch(const D2StatDispatchSeam50C30* s, int32_t nStat, uint32_t nSelector, const uint32_t* pHolder)
{
    if (!pHolder) return 0;
    const uint32_t pUnit = *pHolder;
    if (!pUnit || nStat < 0 || nStat >= s->nTableCount) return 0;
    if (nStat == 0x13) return s->pfnAttackRating((uint32_t)nStat, pUnit);
    if (nSelector == 1) return s->pfnStat38C20(pUnit, nStat, 0);
    if (nSelector == 2) return s->pfnStat39430(pUnit, nStat, 0);
    return s->pfnStat38B70(pUnit, nStat, 0);
}
