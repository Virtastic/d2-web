// 1.13c D2Common+0x539E0 Unload the monster sequence tables. A loaded MonSeq bin is freed with its linker and
// only its record count is cleared (the bin slot is left as it was); a built sequence table is freed and only its
// count is cleared as well.
#include "d2common_montblfree.h"

namespace {
void EngineFreeMonTbls(void* p, int32_t nLine) { FOG_FreePool(nullptr, p, "MonsterTbls.cpp", nLine, 0); }
void EngineFree(void* p, int32_t nLine) { FOG_FreePool(nullptr, p, "DataTbls.cpp", nLine, 0); }
void EngineFreeLinker(D2TxtLinkStrc* p) { FOG_FreeLinker(p); }
const D2MonTblFreeSeam kEngine = { { EngineFree, EngineFreeLinker }, EngineFreeMonTbls };
}
const D2MonTblFreeSeam* DATATBLS_EngineMonTblFreeSeam() { return &kEngine; }

void DATATBLS_UnloadMonSeqTxtSeam(const D2MonTblFreeSeam* pSeam)
{
	if (sgptDataTables->pMonSeqTxt)
	{
		DATATBLS_UnloadBinSeam(&pSeam->tbl, sgptDataTables->pMonSeqTxt);
		pSeam->tbl.pfnFreeLinker(sgptDataTables->pMonSeqLinker);
		sgptDataTables->nMonSeqTxtRecordCount = 0;
	}
	if (sgptDataTables->pMonSeqTable)
	{
		pSeam->pfnFreeMonTbls(sgptDataTables->pMonSeqTable, 0xCEA);
		sgptDataTables->nMonSeqTableRecordCount = 0;
	}
}
