// 1.13c D2Common+0x5E480: monster display name. The unit type (ECX) is unused; the class id is in EDX,
// the output buffer in ESI. A signed id outside monsters.txt, or a row whose wNameStr is the 0x1506
// sentinel, becomes "monster %d"; otherwise the name string is narrowed. Prior art: the 1.13c decompile
#include <cstdint>
#include <cstdio>

#include "D2DataTbls.h"
#include "D2Lang.h"
#include "D2Unicode.h"
#include "DataTbls/MonsterTbls.h"

static_assert(sizeof(D2MonStatsTxt) == 0x1A8, "monstats.txt stride");
static_assert(offsetof(D2MonStatsTxt, wNameStr) == 0x06, "monstats.txt wNameStr");

// 1.13c D2Common+0x5E480
void D2Common_5E480_GetMonsterName(int32_t nClassId, char* szOut)
{
	if (nClassId >= 0 && nClassId < sgptDataTables->nMonStatsTxtRecordCount)
	{
		const D2MonStatsTxt* pRecord = sgptDataTables->pMonStatsTxt + nClassId;
		if (pRecord && pRecord->wNameStr != 5382)
		{
			Unicode::unicode2Win(szOut, D2LANG_GetStringFromTblIndex(pRecord->wNameStr), 0x40);
			return;
		}
	}

	std::sprintf(szOut, "monster %d", static_cast<int>(nClassId));
}
