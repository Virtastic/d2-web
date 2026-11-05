// A monster in anim mode 0xC (or mode 0 when the caller allows it) whose state bits miss the table mask, whose class
// carries MonStats2 flag bit 7, and whose MonStats row has a non-zero velocity word.
#include "d2common_monmode.h"

// 1.13c D2Common+0x50150
uint32_t D2Common_50150_MonModeRowCheck(const D2MonModeSeam* s, uint32_t pUnit, const D2MonModeUnit* u, int32_t nAllowNeutral)
{
    if (!pUnit || u->nType != 1) return 0;
    if (u->nMode != 0xC && (nAllowNeutral == 0 || u->nMode != 0)) return 0;
    if (s->pfnStateMaskHit(pUnit, s->pMask) != 0) return 0;
    const int32_t nClass = static_cast<int32_t>(u->nClass);
    if (s->pfnMonFlag(nClass, 7) == 0) return 0;
    // an id outside the table reads a short at absolute address 0x32 in retail (a fault); it answers 0 here
    if (nClass < 0 || nClass >= s->nRows || !s->pRows) return 0;
    int16_t v; const uint8_t* p = s->pRows + nClass * 0x1A8 + 0x32;
    v = static_cast<int16_t>(p[0] | (p[1] << 8));
    return v != 0 ? 1u : 0u;
}
