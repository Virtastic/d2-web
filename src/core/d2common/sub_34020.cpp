// 1.13c D2Common+0x33C20 and +0x34020 (#10341)
#include <cstdint>
#include <cstddef>
#include "D2DataTbls.h"
#include "D2StatList.h"
#include "Units/Units.h"
static_assert(sizeof(void*) != 4 || offsetof(D2UnitStrc, pStatListEx) == 0x5C, "unit statlist");   // retail (32-bit) offset; our wasm64 structs carry wider pointers
static_assert(sizeof(void*) != 4 || offsetof(D2StatListStrc, dwFlags) == 0x10, "statlist flags");   // retail (32-bit) offset; our wasm64 structs carry wider pointers
static_assert(sizeof(void*) != 4 || offsetof(D2StatListExStrc, StatFlags) == 0x58, "statlistex state bits");   // retail (32-bit) offset; our wasm64 structs carry wider pointers
static_assert(sizeof(void*) != 4 || offsetof(D2DataTablesStrc, nStatesTxtRecordCount) == 0xC4, "state count");   // retail (32-bit) offset; our wasm64 structs carry wider pointers
static_assert(sizeof(void*) != 4 || offsetof(D2DataTablesStrc, fStateMasks) == 0xCC, "state masks");   // retail (32-bit) offset; our wasm64 structs carry wider pointers
// ponytail: retail offsets are asserted only where pointers are 4 bytes; the 1.13c offsets are in the spec.
// 0x33C20 (eax = unit, ecx = mask): 1 if the unit's extended statlist has a state bit in mask. Same as STATES_CheckStateMaskOnUnit minus the index check.
uint32_t D2Common_33C20_UnitHasStateInMask(D2UnitStrc* pUnit, const uint32_t* pMask)
{
    const uint32_t* pBits = D2COMMON_STATES_GetStatFlags_6FDB8A90(pUnit);
    if (!pBits) return 0;
    for (int i = 0, n = (sgptDataTables->nStatesTxtRecordCount + 31) / 32; i < n; ++i)
        if (pBits[i] & pMask[i]) return 1;
    return 0;
}
// 0x34020 stdcall(unit): the mask is fStateMasks[2] (DataTables+0xD4).
uint32_t D2Common_34020(D2UnitStrc* pUnit) { return D2Common_33C20_UnitHasStateInMask(pUnit, sgptDataTables->fStateMasks[2]); }
