// 1.13c D2Common+0x5D8B0: release the five DataTbls blocks (lines 0x6E8, 0x6ED, 0x6F2,
// 0x6F7, 0x6FC, each cleared after its free), zero the 0x28-dword state block, free the base block at line 0x8F8 (flag
// zero) or 0x8F6 (flag set, pointer minus 4), then release the linker. Fog frees and the linker free are host callbacks.
#include <cstdint>

struct D2Common5D8B0State {
    uintptr_t pBlk[5];     // DAT_6FDEFFA0, DAT_6FDF0044, DAT_6FDF004C, DAT_6FDF0054, DAT_6FDF005C
    uintptr_t pBase;       // DAT_6FDEFF94
    uint32_t  bFlag;       // DAT_6FDE9E20 (0x99E20)
    uintptr_t pLinker;     // DAT_6FDEFF98: linker, freed by 0x9246 and not cleared
};
typedef void (*D2Common5D8B0Free)(uintptr_t p, int32_t nFile, int32_t nLine);   // Fog free 0x9228 (nFile 1 = DataTbls.cpp)
typedef void (*D2Common5D8B0LinkerFree)(uintptr_t p);                           // 0x9246

// 1.13c D2Common+0x5D8B0. pZeroBlk is the 0x28-dword block at DAT_6FDEFFA4.
extern "C" void D2Common_5D8B0_Release(D2Common5D8B0State* s, uint32_t* pZeroBlk, D2Common5D8B0Free pFree, D2Common5D8B0LinkerFree pLinkerFree)
{
    static const int32_t kLine[5] = { 0x6E8, 0x6ED, 0x6F2, 0x6F7, 0x6FC };
    for (int i = 0; i < 5; ++i) {
        if (s->pBlk[i] != 0) {
            pFree(s->pBlk[i], 1, kLine[i]);
            s->pBlk[i] = 0;
        }
    }
    for (int i = 0; i < 0x28; ++i) pZeroBlk[i] = 0;
    if (s->pBase != 0) {
        const int32_t nLine = s->bFlag == 0 ? 0x8F8 : 0x8F6;
        const uintptr_t nBias = s->bFlag == 0 ? 0 : 4;
        pFree(s->pBase - nBias, 1, nLine);
    }
    pLinkerFree(s->pLinker);
}
