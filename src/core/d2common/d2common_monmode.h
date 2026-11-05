// Monster-unit mode predicates (the anim-mode 0xC checks): the callees and the data-table words they read are seams,
// so the rule runs on the engine tables or on a recorded image of them.
#pragma once
#include <cstdint>

struct D2MonModeSeam {
    int32_t (*pfnStateMaskHit)(uint32_t pUnit, uint32_t pMask);   // the unit's state bits against a table mask
    int32_t (*pfnMonFlag)(int32_t nClass, int32_t nBit);            // a MonStats2 flag bit of a monster class
    uint32_t pMask;                                                  // the data-table mask the state test gets
    int32_t nRows;                                                   // MonStats row count
    const uint8_t* pRows;                                            // MonStats rows, 0x1A8 bytes each
};
struct D2MonModeUnit { uint32_t nType, nClass, nMode; };            // unit type, class id and anim mode of pUnit

uint32_t D2Common_500E0_MonModeCFlagsCheck(const D2MonModeSeam* s, uint32_t pUnit, const D2MonModeUnit* u);
uint32_t D2Common_50150_MonModeRowCheck(const D2MonModeSeam* s, uint32_t pUnit, const D2MonModeUnit* u, int32_t nAllowNeutral);
