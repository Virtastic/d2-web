// 1.13c D2Common+0x4A450 (#10173) ProfCore reset: dump the table (0x4A3D0, the log), then zero the
// delta and count of all 23 records; the rest of each record is kept.
#include "d2common_prof.h"

void D2Common_4A450_ProfileReset(D2ProfSlot* pSlots, void (*pfnDump)())
{
	pfnDump();
	for (int32_t i = 0; i < kD2ProfSlots; ++i)
	{
		pSlots[i].nLow = 0;
		pSlots[i].nHigh = 0;
		pSlots[i].nCount = 0;
	}
}
