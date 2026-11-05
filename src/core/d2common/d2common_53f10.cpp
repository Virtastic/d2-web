// 1.13c D2Common+0x53F10 DATATBLS_UnloadSuperUniquesTxt: the linker is freed but its slot is left
// stale; a loaded table is freed and its slot cleared; the record count is zeroed.
#include "d2common_tblfree.h"
void DATATBLS_UnloadSuperUniquesTxtSeam(const D2TblFreeSeam* pSeam)
{
	if (sgptDataTables->pSuperUniquesLinker) pSeam->pfnFreeLinker(sgptDataTables->pSuperUniquesLinker);
	if (sgptDataTables->pSuperUniquesTxt)
	{
		DATATBLS_UnloadBinSeam(pSeam, sgptDataTables->pSuperUniquesTxt);
		sgptDataTables->pSuperUniquesTxt = nullptr;
	}
	sgptDataTables->nSuperUniquesTxtRecordCount = 0;
}
