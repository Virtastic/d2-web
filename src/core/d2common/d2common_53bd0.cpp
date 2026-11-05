// 1.13c D2Common+0x53BD0 Unload MonType: when the bin is loaded it is freed, then its linker, then the nest
// array (whatever it holds, null included); the bin and linker slots are cleared, the nest slot is left.
#include "d2common_montblfree.h"

void DATATBLS_UnloadMonTypeTxtSeam(const D2MonTblFreeSeam* pSeam)
{
	if (!sgptDataTables->pMonTypeTxt)
		return;
	DATATBLS_UnloadBinSeam(&pSeam->tbl, sgptDataTables->pMonTypeTxt);
	pSeam->tbl.pfnFreeLinker(sgptDataTables->pMonTypeLinker);
	pSeam->pfnFreeMonTbls(sgptDataTables->pMonTypeNest, 0xB16);
	sgptDataTables->pMonTypeTxt = nullptr;
	sgptDataTables->pMonTypeLinker = nullptr;
}

extern "C" void d2_batch_d2common_53bd0() { DATATBLS_UnloadMonTypeTxtSeam(DATATBLS_EngineMonTblFreeSeam()); }
