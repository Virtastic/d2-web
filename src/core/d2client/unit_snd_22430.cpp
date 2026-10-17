// 1.13c D2Client+0x22430 (UnitSnd.cpp) --
// ESI = pUnit (NULL allowed). Returns the unit type the sound paths branch on: a shape-shifted player's
// type is converted (type 1 = monster form) by D2Common ordinal 10350 first. No globals are read.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

#include "../d2moo/include/D2Skills.h"
#include "../d2moo/include/Units/Units.h"

static_assert(UNIT_PLAYER == 0 && UNIT_MONSTER == 1, "D2C_UnitTypes");
static_assert(offsetof(D2UnitStrc, dwUnitType) == 0x00, "D2UnitStrc::dwUnitType");
static_assert(offsetof(D2UnitStrc, dwClassId) == 0x04, "D2UnitStrc::dwClassId");
static_assert(offsetof(D2UnitStrc, dwAnimMode) == 0x10 +  1 * (sizeof(void*) - 4), "D2UnitStrc::dwAnimMode");
static_assert(offsetof(D2UnitStrc, pAnimSeq)   == 0x30 +  4 * (sizeof(void*) - 4), "D2UnitStrc::pAnimSeq");
static_assert(offsetof(D2UnitStrc, dwSeqMode)  == 0x40 +  5 * (sizeof(void*) - 4), "D2UnitStrc::dwSeqMode");
static_assert(offsetof(D2UnitStrc, dwFlagEx)   == 0xC8 + 18 * (sizeof(void*) - 4), "D2UnitStrc::dwFlagEx");
static_assert((int)UNITFLAGEX_ISSHAPESHIFTED == 0x00000008, "UNITFLAGEX_ISSHAPESHIFTED");

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_22430(D2UnitStrc* pUnit)
{
    int32_t nType = 6;      // 0x6: the type a NULL unit reports (retail's literal in EAX)
    int32_t nClass = -1;    // -1: ditto for the class
    if (pUnit) {
        nType = (int32_t)pUnit->dwUnitType;
        nClass = (int32_t)pUnit->dwClassId;
    }
    int32_t nMode = UNITS_GetAnimOrSeqMode(pUnit);
    // Only a player is offered to the converter, which rewrites nType (and nClass as out-params) when the
    // unit is shape-shifted; the class/mode it writes back are dead here -- only nType is returned.
    if (nType == UNIT_PLAYER) {
        D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, __FILE__, __LINE__);
    }
    return nType;
}
