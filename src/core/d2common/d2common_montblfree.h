// Free seam of the monster-table teardowns that release both a DataTbls-style bin (through D2TblFreeSeam) and a
// MonsterTbls.cpp allocation (a plain pool free tagged with that file's line). The engine passes Fog; FN probes pass recorders.
#pragma once
#include "d2common_tblfree.h"
struct D2MonTblFreeSeam {
	D2TblFreeSeam tbl;
	void (*pfnFreeMonTbls)(void* p, int32_t nLine);
};
const D2MonTblFreeSeam* DATATBLS_EngineMonTblFreeSeam();
void DATATBLS_UnloadMonSeqTxtSeam(const D2MonTblFreeSeam* pSeam);
void DATATBLS_UnloadMonTypeTxtSeam(const D2MonTblFreeSeam* pSeam);
