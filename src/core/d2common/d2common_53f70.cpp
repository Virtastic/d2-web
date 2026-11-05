// 1.13c D2Common+0x53F70 DATATBLS_UnloadMonUModTxt: the linker is freed and cleared, a loaded table
// is freed and cleared, the record count is zeroed.
#include "d2common_tblfree.h"
void DATATBLS_UnloadMonUModTxtSeam(const D2TblFreeSeam* pSeam)
{
	if (sgptDataTables->pMonUModLinker)
	{
		pSeam->pfnFreeLinker(sgptDataTables->pMonUModLinker);
		sgptDataTables->pMonUModLinker = nullptr;
	}
	if (sgptDataTables->pMonUModTxt)
	{
		DATATBLS_UnloadBinSeam(pSeam, sgptDataTables->pMonUModTxt);
		sgptDataTables->pMonUModTxt = nullptr;
	}
	sgptDataTables->nMonUModTxtRecordCount = 0;
}
