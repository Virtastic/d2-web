// 1.13c D2Common+0x5E4D0: player class display name. Class id in ECX, output buffer in EDX. A signed id
// outside charstats.txt, or a row with an empty class name, becomes "player %d"; otherwise the fixed class
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "D2DataTbls.h"

static_assert(sizeof(D2CharStatsTxt) == 0xC4, "charstats.txt stride");
static_assert(offsetof(D2CharStatsTxt, szClassName) == 0x20, "charstats.txt class name");

// 1.13c D2Common+0x5E4D0
void D2Common_5E4D0_GetPlayerName(int32_t nClassId, char* szOut)
{
	if (nClassId >= 0 && nClassId < sgptDataTables->nCharStatsTxtRecordCount)
	{
		const D2CharStatsTxt* pRecord = sgptDataTables->pCharStatsTxt + nClassId;
		if (pRecord && pRecord->szClassName[0])
		{
			std::strcpy(szOut, pRecord->szClassName);
			return;
		}
	}

	std::sprintf(szOut, "player %d", static_cast<int>(nClassId));
}
