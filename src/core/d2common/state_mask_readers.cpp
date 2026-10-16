// 1.13c D2Common+0x33A00 / +0x33B40 / +0x33BA0 / +0x33BE0: single-bit readers of the state-mask tables in sgptDataTables
//. Sibling: sub_33B00 in itemtype_getters.cpp.
#include <cstdint>
#include "D2DataTbls.h"
#include "D2States.h"
#include "D2BitManip.h"
static uint32_t Bit(int32_t m, int32_t id) { return sgptDataTables->fStateMasks[m][id >> 5] & gdwBitMasks[id & 31]; }
static uint32_t BitChecked(int32_t m, int32_t id) { return (id >= 0 && id < sgptDataTables->nStatesTxtRecordCount) ? Bit(m, id) : 0; }
uint32_t D2Common_33A00_StateMaskHide(int32_t id) { return Bit(STATEMASK_HIDE, id); } // 1.13c: no bounds check
uint32_t D2Common_33B40_StateMaskCurse(int32_t id) { return BitChecked(STATEMASK_CURSE, id); }
uint32_t D2Common_33BA0_StateMaskPgsv(int32_t id) { return BitChecked(STATEMASK_PGSV, id); }
uint32_t D2Common_33BE0_StateMaskAura(int32_t id) { return BitChecked(STATEMASK_AURA, id); }
