// 1.13c D2Common+0x18F90: release of the five token/table blocks, each guarded by its own non-null test
// and cleared afterwards. Free order: plrmode slot (TokTbls.cpp:0xD6), monmode (DataTbls.cpp), objmode
// (TokTbls.cpp:0xE2), composit (DataTbls.cpp), armtype (DataTbls.cpp). The DataTbls.cpp line is 0x8F8
// while the flag at 0x99E20 is zero and 0x8F6 otherwise. The free is a host callback.
#include <cstdint>

struct D2Common18F90Slots {
    uintptr_t pPlrMode, pMonMode, pObjMode, pComposit, pArmType;   // 0x1A13C8, 0x1A13A8, 0x1A13B8, 0x1A13D8, 0x1A13D4
    uint32_t bFlag;                                                 // 0x99E20
};
typedef void (*D2Common18F90Free)(uintptr_t p, int32_t nFile, int32_t nLine);   // nFile 0 = TokTbls.cpp, 1 = DataTbls.cpp

// 1.13c D2Common+0x18F90
extern "C" void D2Common_18F90_Release(D2Common18F90Slots* s, D2Common18F90Free pFree)
{
    const int32_t nDataLine = s->bFlag == 0 ? 0x8F8 : 0x8F6;
	const uintptr_t nBias = s->bFlag == 0 ? 0 : 4;   // flag set: the DataTbls frees get the block address minus 4
    if (s->pPlrMode) pFree(s->pPlrMode, 0, 0xD6);
    s->pPlrMode = 0;
    if (s->pMonMode) pFree(s->pMonMode - nBias, 1, nDataLine);
    s->pMonMode = 0;
    if (s->pObjMode) pFree(s->pObjMode, 0, 0xE2);
    s->pObjMode = 0;
    if (s->pComposit) pFree(s->pComposit - nBias, 1, nDataLine);
    s->pComposit = 0;
    if (s->pArmType) pFree(s->pArmType - nBias, 1, nDataLine);
    s->pArmType = 0;
}
