// The free seam of the 1.13c table-teardown routines (0x53AD0, 0x53EB0, 0x53F10, 0x53F70): retail inlines DATATBLS_UnloadBin
// as a Fog #10046 free of the slot (minus the 4-byte .bin header when the tables came from .bin files, DataTbls.cpp line 0x8F6;
// the slot itself otherwise, line 0x8F8) and frees linkers through Fog #10212. The engine passes the Fog calls; the FN probes
// pass recorders.
#pragma once
#include <cstdint>
#include "D2DataTbls.h"
struct D2TblFreeSeam {
	void (*pfnFree)(void* p, int32_t nLine);
	void (*pfnFreeLinker)(D2TxtLinkStrc* pLinker);
};
inline void DATATBLS_UnloadBinSeam(const D2TblFreeSeam* pSeam, void* pBin)
{
	if (DATATBLS_LoadFromBin) pSeam->pfnFree((char*)pBin - 4, 0x8F6);
	else pSeam->pfnFree(pBin, 0x8F8);
}
const D2TblFreeSeam* DATATBLS_EngineTblFreeSeam();
void DATATBLS_UnloadHirelingTxtSeam(const D2TblFreeSeam* pSeam);
void DATATBLS_UnloadMonPresetTxtSeam(const D2TblFreeSeam* pSeam);
void DATATBLS_UnloadSuperUniquesTxtSeam(const D2TblFreeSeam* pSeam);
void DATATBLS_UnloadMonUModTxtSeam(const D2TblFreeSeam* pSeam);
