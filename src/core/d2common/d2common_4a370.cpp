// 1.13c D2Common+0x4A370 (#10021) ProfCore accumulate: slot ECX gains (counter now - previous) as a
// 64-bit delta and one call. The counter is QueryPerformanceCounter in retail; here a seam.
#include "d2common_prof.h"

D2ProfSlot* D2Common_ProfSlots()
{
	static D2ProfSlot s_slots[kD2ProfSlots];
	return s_slots;
}

void D2Common_4A370_PerfAccumulate(D2ProfSlot* pSlots, int32_t nSlot, uint32_t nPreviousLow, uint32_t nPreviousHigh,
	void (*pfnCounter)(uint32_t* pLowHigh))
{
	uint32_t now[2] = { 0, 0 };
	pfnCounter(now);
	D2ProfSlot& slot = pSlots[nSlot];   // retail indexes without a bound check
	const uint64_t delta = ((uint64_t)now[1] << 32 | now[0]) - ((uint64_t)nPreviousHigh << 32 | nPreviousLow);
	const uint64_t sum = ((uint64_t)slot.nHigh << 32 | slot.nLow) + delta;
	slot.nLow = (uint32_t)sum;
	slot.nHigh = (uint32_t)(sum >> 32);
	++slot.nCount;
}
