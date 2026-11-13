// Stat-value dispatcher reached through a pointer table (the selector is the second register). ECX = stat id, EDX = selector,
// stack: one unused word and a pointer to a holder whose first dword is the unit (ret 8). A null holder, a null unit, a negative
// stat id or one at or past the stat-table count (sgptDataTables+0xBD4) returns 0. Stat 0x13 goes to the attack-rating routine (0x31EA0,
// thiscall, ECX still 0x13, one stack word = the unit). Otherwise selector 1 -> 0x38C20, 2 -> 0x39430, anything else -> 0x38B70, each
// as (unit, stat id, layer 0) with three stack words. The 0x50C30 row is the same code at another address.
#include <cstdint>

struct D2StatDispatchSeam {
    int32_t (*pfnAttackRating)(uint32_t nEcx, uint32_t pUnit);
    int32_t (*pfnStat38B70)(uint32_t pUnit, int32_t nStat, int32_t nLayer);
    int32_t (*pfnStat38C20)(uint32_t pUnit, int32_t nStat, int32_t nLayer);
    int32_t (*pfnStat39430)(uint32_t pUnit, int32_t nStat, int32_t nLayer);
    int32_t nTableCount;   // sgptDataTables + 0xBD4
};

// 1.13c D2Common+0x27120
extern "C" int32_t D2Common_27120_StatDispatch(const D2StatDispatchSeam* s, int32_t nStat, uint32_t nSelector, const uint32_t* pHolder)
{
    if (!pHolder) return 0;
    const uint32_t pUnit = *pHolder;
    if (!pUnit || nStat < 0 || nStat >= s->nTableCount) return 0;
    if (nStat == 0x13) return s->pfnAttackRating((uint32_t)nStat, pUnit);
    if (nSelector == 1) return s->pfnStat38C20(pUnit, nStat, 0);
    if (nSelector == 2) return s->pfnStat39430(pUnit, nStat, 0);
    return s->pfnStat38B70(pUnit, nStat, 0);
}
