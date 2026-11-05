// 1.13c D2Common+0x53AD0 DATATBLS_UnloadMonPresetTxt: a loaded monpreset table is freed (the slot
// keeps its stale pointer), then the five per-act section pointers and their record counts are zeroed.
#include "d2common_tblfree.h"
void DATATBLS_UnloadMonPresetTxtSeam(const D2TblFreeSeam* pSeam)
{
	if (sgptDataTables->pMonPresetTxt) DATATBLS_UnloadBinSeam(pSeam, sgptDataTables->pMonPresetTxt);
	for (int32_t i = 0; i < 5; ++i)
	{
		sgptDataTables->pMonPresetTxtActSections[i] = nullptr;
		sgptDataTables->nMonPresetTxtActRecordCounts[i] = 0;
	}
}
