// 1.13c D2Common+0x3AC50 (#11040), moved out of d2common_batch_82030.cpp into its own file.
#include <cstdint>
#include <emscripten.h>

#include "D2StatList.h"
#include "Units/Units.h"

// 0x37F40 seam is defined in d2common_batch_82030.cpp; 0x3A650 is implemented in d2common_batch_37ca0.cpp.
extern "C" uint32_t (*g_d2common_fn_37F40)(uint32_t);
extern "C" void D2Common_3A650_DropListEntries(uint32_t pList);

// Clear bit 8 of the unit's stat-list dwFlags (+0x10). If the list is still extended (bit 31), allocate a
// 0x80-byte block (0x37F40) and hand it to 0x3A650. A null unit or stat list, or a non-extended list, does
// nothing; a failed allocation is ignored.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_3AC50_ClearStatListFlag(D2UnitStrc* pUnit) {  // 1.13c D2Common+0x3AC50 (#11040)
	if (pUnit == nullptr || pUnit->pStatListEx == nullptr)
		return;

	D2StatListExStrc* pStatList = pUnit->pStatListEx;
	pStatList->dwFlags &= 0xFFFFFEFFu;

	if (static_cast<int32_t>(pStatList->dwFlags) >= 0 || g_d2common_fn_37F40 == nullptr)
		return;
	const uint32_t pBlock = g_d2common_fn_37F40(0x80);
	if (pBlock != 0)
		D2Common_3A650_DropListEntries(pBlock);
}
