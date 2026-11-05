// A monster in anim mode 0xC whose state bits miss the table mask and whose class carries MonStats2 flag bits 7 and 0xC.
#include "d2common_monmode.h"

// 1.13c D2Common+0x500E0
uint32_t D2Common_500E0_MonModeCFlagsCheck(const D2MonModeSeam* s, uint32_t pUnit, const D2MonModeUnit* u)
{
    if (!pUnit || u->nType != 1 || u->nMode != 0xC) return 0;
    if (s->pfnStateMaskHit(pUnit, s->pMask) != 0) return 0;
    const int32_t nClass = static_cast<int32_t>(u->nClass);
    if (s->pfnMonFlag(nClass, 7) == 0) return 0;
    return s->pfnMonFlag(nClass, 0xC) != 0 ? 1u : 0u;
}
