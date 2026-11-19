// Moved from the stand-alone d2common batch into the game tree (treasure-class loader leaf of DATATBLS_LoadTreasureClassExTxt).
// Prior art: 1.10f inlined body, vendored as DATATBLS_NewTreasureClassEx.
#include <cstdint>
#include <test_seam.h>
#include <cstring>
#include "D2DataTbls.h"
#include "DataTbls/MonsterTbls.h"

// 1.13c D2Common+0x53780: register a treasure-class name in the linker, grow the record array in steps of
// 16, zero the new slot, bump the count, return the slot. (The vendored the 1.10f source twin leaves the bump to the caller.)
D2TCExShortStrc* D2Common_53780_NewTreasureClassEx(const char* szName)
{
	if (D2Seam<&D2Common_53780_NewTreasureClassEx>::hook) return D2Seam<&D2Common_53780_NewTreasureClassEx>::hook(szName);
	FOG_10216_AddRecordToLinkingTable(sgptDataTables->pTreasureClassExLinker, szName);
	if (!(sgptDataTables->nTreasureClassEx % 16))
	{
		sgptDataTables->pTreasureClassEx = (D2TCExShortStrc*)D2_REALLOC_POOL(nullptr, sgptDataTables->pTreasureClassEx,
			sizeof(D2TCExShortStrc) * (sgptDataTables->nTreasureClassEx + 16));
	}
	D2TCExShortStrc* pRecord = &sgptDataTables->pTreasureClassEx[sgptDataTables->nTreasureClassEx];
	memset(pRecord, 0x00, sizeof(*pRecord));
	++sgptDataTables->nTreasureClassEx;
	return pRecord;
}
