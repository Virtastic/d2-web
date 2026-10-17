// 1.13c D2Client+0x22490: ESI = pUnit, plain ret -> the unit's class
// (+0x04), after D2COMMON_11013_ConvertMode (#10350) for a player; UNITS_GetAnimOrSeqMode (#10175) is
// entered unconditionally (NULL included) and its result pre-seeds the mode #10350 reads back.
// Siblings 0x223D0 (mode) and 0x2430 (type) share the prologue and the same nType==0 gate.
#include <cstdint>
#include "../d2moo/include/D2Skills.h"
#include "../d2moo/include/Units/Units.h"

// ev (optional; the capture's protocol): ev[0] = number of callee entries, ev[1 + i] = the RVA of the
// i-th callee entered, in entry order (0xC65E = #10175, 0xC658 = #10350).
extern "C" int32_t d2_client_22490(D2UnitStrc* pUnit, uint32_t* ev)
{
    int32_t nType = 6;      // the literals a NULL unit leaves for the callee
    int32_t nClass = -1;
    int32_t nMode;
    if (ev) ev[0] = 0;
    if (pUnit) {
        nType = (int32_t)pUnit->dwUnitType;
        nClass = (int32_t)pUnit->dwClassId;
    }
    if (ev) ev[1 + ev[0]++] = 0xC65E;
    nMode = UNITS_GetAnimOrSeqMode(pUnit);
    if (nType == UNIT_PLAYER) {
        if (ev) ev[1 + ev[0]++] = 0xC658;
        D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, __FILE__, __LINE__);
    }
    return nClass;
}
