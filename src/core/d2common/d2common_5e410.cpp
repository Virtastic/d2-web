// 1.13c D2Common+0x5E410: item display name. Item id in ECX (unsigned compare), output buffer in ESI.
// An id at or above the items.txt count becomes "item %d". In range, a null items.txt table is a fatal
// path in retail (0x1EF, then _exit(-1)), modelled as a return. A row whose wNameStr is not the 0x1506
// sentinel is narrowed from the string table; otherwise the id is formatted. Prior art: the 1.13c decompile
#include <cstdint>
#include <cstdio>

#include "D2DataTbls.h"
#include "D2Lang.h"
#include "D2Unicode.h"
#include "DataTbls/ItemsTbls.h"

static_assert(sizeof(D2ItemsTxt) == 0x1A8, "items.txt stride");
static_assert(offsetof(D2ItemsTxt, wNameStr) == 0xF4, "items.txt wNameStr");

// 1.13c D2Common+0x5E410
void D2Common_5E410_GetItemName(int32_t nItemId, char* szOut)
{
	const D2ItemDataTbl& itemData = sgptDataTables->pItemDataTables;
	if (static_cast<uint32_t>(nItemId) < static_cast<uint32_t>(itemData.nItemsTxtRecordCount))
	{
		if (!itemData.pItemsTxt)
			return; // retail fatal 0x1EF, then _exit(-1)

		const D2ItemsTxt* pRecord = itemData.pItemsTxt + nItemId;
		if (pRecord && pRecord->wNameStr != 5382)
		{
			Unicode::unicode2Win(szOut, D2LANG_GetStringFromTblIndex(pRecord->wNameStr), 0x40);
			return;
		}
	}

	std::sprintf(szOut, "item %d", static_cast<int>(nItemId));
}
