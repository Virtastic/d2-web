// 1.13c D2Common+0x53EB0 DATATBLS_UnloadHirelingTxt: a loaded hireling table is freed (the slot
// keeps its stale pointer), the record count is zeroed and both 256-entry start-record id arrays are set to -1.
#include "d2common_tblfree.h"
void DATATBLS_UnloadHirelingTxtSeam(const D2TblFreeSeam* pSeam)
{
	if (sgptDataTables->pHirelingTxt) DATATBLS_UnloadBinSeam(pSeam, sgptDataTables->pHirelingTxt);
	sgptDataTables->nHirelingTxtRecordCount = 0;
	for (int32_t& nId : sgptDataTables->nHirelingStartRecordIds) nId = -1;
}
