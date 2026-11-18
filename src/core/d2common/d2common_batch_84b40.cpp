// D2Common implementation batch: +0x84B40, +0x4EBA0, +0x30160, +0x4FF80 translated from the pinned
// 1.13c decompiles. Vendored helpers are called by name; the global drop-room table is an env seam.
#include <cstddef>
#include <cstdint>

#include <emscripten.h>

#include "D2BitManip.h"
#include "D2DataTbls.h"
#include "D2StatList.h"
#include "DataTbls/MonsterTbls.h"
#include "Units/Units.h"

// 0x33C20, implemented in src/core/d2common/sub_33c20.cpp.
uint32_t D2Common_33C20_BitmaskHit(bool hasOwner, bool hasRec, int32_t recFlag,
	const uint32_t* bits, const uint32_t* mask, int32_t nWords);

// The 1.13c drop-room pointer table and its entry count. The table is filled by the data loader;
// zero means no entry is reachable yet.
extern "C" uint32_t* g_d2common_dat_6fdeff54 = nullptr;
extern "C" int32_t g_d2common_dat_6fdeff5c = 0;

// D2GetDropRoom (0x84B40) and the state-bit test (0x37DB0) live in the game tree, src/core/d2moo/src/Retail.
extern uint32_t D2Common_37DB0_UnitStateBit(const D2UnitStrc* pUnit, int32_t nBit);

namespace
{
	uint32_t Batch2_UnitStateBit(const D2UnitStrc* pUnit, int32_t nBit) { return D2Common_37DB0_UnitStateBit(pUnit, nBit); }
}

// 1.13c D2Common+0x4EBA0 (#10497): 2 when the unit is one of the three state-bearing types and state
// 0x86 (checked through 0x37DB0) is set; 0 otherwise.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2common_4EBA0_UnitState86(D2UnitStrc* pUnit)
{
	if (!pUnit) return 0;

	const int32_t nType = (int32_t)pUnit->dwUnitType;
	if (nType < 0) return 0;
	if (!(nType < 2 || nType == 3)) return 0;
	if (sgptDataTables->nStatesTxtRecordCount <= 0x86) return 0;

	return Batch2_UnitStateBit(pUnit, 0x86) ? 2 : 0;
}

namespace
{
	// 0x1470: MonStats2 dwFlags (+0x04) bit nBit of the monster row.
	uint32_t Batch2_MonStats2FlagBit(int32_t nMonsterId, int32_t nBit)
	{
		if (nMonsterId < 0 || nMonsterId >= sgptDataTables->nMonStatsTxtRecordCount) return 0;
		if (!sgptDataTables->pMonStatsTxt) return 0;

		const int32_t nMonStats2 = sgptDataTables->pMonStatsTxt[nMonsterId].wMonStatsEx;
		if (nMonStats2 < 0 || nMonStats2 >= sgptDataTables->nMonStats2TxtRecordCount) return 0;
		if (!sgptDataTables->pMonStats2Txt) return 0;

		return sgptDataTables->pMonStats2Txt[nMonStats2].nFlags[nBit >> 3] & gdwBitMasks[nBit & 7];
	}
}

// 1.13c D2Common+0x4FF80 (#10501): 1 when the monster in anim mode 12 is in none of state-mask 33
// and has MonStats2 flag bit 7 set. The decompiler loses the class-id register of the 0x1470 call;
// the unit's own class id is the only source. Returns 0 otherwise.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_4FF80_MonsterAnim12NoStateFlag7(D2UnitStrc* pUnit)
{
	if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER) return 0;
	if (pUnit->dwAnimMode != 0xC) return 0;

	D2StatListExStrc* pStatList = pUnit->pStatListEx;
	const bool bHasRec = pStatList != nullptr;
	const int32_t nRecFlag = pStatList ? (int32_t)pStatList->dwFlags : 0;
	const uint32_t* pBits = pStatList ? pStatList->StatFlags : nullptr;
	const int32_t nWords = (sgptDataTables->nStatesTxtRecordCount + 31) / 32;

	if (D2Common_33C20_BitmaskHit(true, bHasRec, nRecFlag, pBits,
		sgptDataTables->fStateMasks[33], nWords) != 0)
	{
		return 0;
	}

	return Batch2_MonStats2FlagBit(pUnit->dwClassId, 7) ? 1 : 0;
}
