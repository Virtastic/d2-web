// 1.13c D2Client+0x223D0: a unit's effective animation mode, for the
// D2Client\UNIT\UnitSnd.cpp sound paths. esi = pUnit; the unit pointer is retail's convention, not a
// stack argument. Callees: D2Common ordinal 10175 (UNITS_GetAnimOrSeqMode) and, for a player, ordinal
// 10350 (D2COMMON_11013_ConvertMode), which converts only a shapeshifted unit.
#include <cstdint>
#include <cstddef>
#include "../d2moo/include/D2Skills.h"        // D2COMMON_11013_ConvertMode
#include "../d2moo/include/Units/Units.h"     // UNITS_GetAnimOrSeqMode, D2UnitStrc

// The unit fields this function copies out before the conversion (the mode itself is the callee's read).
struct D2cUnitMode223d0 {
    uint32_t dwUnitType;                      // +0x00
    int32_t  dwClassId;                       // +0x04
};
static_assert(offsetof(D2cUnitMode223d0, dwUnitType) == 0x00, "dwUnitType");
static_assert(offsetof(D2cUnitMode223d0, dwClassId) == 0x04, "dwClassId");
// This struct is compiled both 32-bit (the capture exe) and wasm64 (the engine): every pointer field is
// 8 bytes there, so each field after a pointer sits (sizeof(void*)-4) further along than retail. The
// retail offset plus one step per pointer field before it holds in both builds (D2MOO's own convention,
// src/core/d2moo/src/Units/Units.cpp:2127).
static_assert(offsetof(D2UnitStrc, dwUnitType) == 0x00, "D2UnitStrc dwUnitType");
static_assert(offsetof(D2UnitStrc, dwClassId)  == 0x04 +  0 * (sizeof(void*) - 4), "D2UnitStrc dwClassId");
static_assert(offsetof(D2UnitStrc, dwAnimMode) == 0x10 +  1 * (sizeof(void*) - 4), "D2UnitStrc dwAnimMode");
static_assert(offsetof(D2UnitStrc, pAnimSeq)   == 0x30 +  4 * (sizeof(void*) - 4), "D2UnitStrc pAnimSeq");
static_assert(offsetof(D2UnitStrc, dwSeqMode)  == 0x40 +  5 * (sizeof(void*) - 4), "D2UnitStrc dwSeqMode");
static_assert(offsetof(D2UnitStrc, dwFlagEx)   == 0xC8 + 18 * (sizeof(void*) - 4), "D2UnitStrc dwFlagEx");

extern "C" int32_t d2_client_223d0(D2UnitStrc* pUnit) {
    // The literals are D2Client's own: a NULL unit leaves 6 (UNIT_TYPES_COUNT) and -1 for the callee.
    int32_t nType  = pUnit ? (int32_t)pUnit->dwUnitType : 6;
    int32_t nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
    int32_t nMode  = UNITS_GetAnimOrSeqMode(pUnit);
    if (nType == 0) {                          // a player only; a NULL unit's 6 skips the call
        // szFile/nLine are the callee's trace text only, so ours stands in for UnitSnd.cpp:2841.
        D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, __FILE__, __LINE__);
    }
    return nMode;
}
