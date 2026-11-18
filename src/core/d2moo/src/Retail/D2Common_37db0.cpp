// State-bit test of a unit's extended stat list: the one body the two d2common batches and the 37DB0 / 33CD0 proof rows share.
#include <cstdint>

#include "D2BitManip.h"
#include "D2StatList.h"
#include "Units/Units.h"

// 1.13c D2Common+0x37DB0: state bit nBit of the unit's stat list. Only an extended list (sign bit of
// dwFlags set) with a state-flag array answers; a missing unit, list or array answers 0.
uint32_t D2Common_37DB0_UnitStateBit(const D2UnitStrc* pUnit, int32_t nBit)
{
	const D2StatListExStrc* pStatList = pUnit ? pUnit->pStatListEx : nullptr;
	if (!pStatList || (int32_t)pStatList->dwFlags >= 0 || !pStatList->StatFlags) return 0;
	return pStatList->StatFlags[nBit >> 5] & gdwBitMasks[nBit & 31];
}
