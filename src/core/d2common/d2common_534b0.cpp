// 1.13c D2Common+0x534B0: release the treasure-class linker at 0x6FDF0890, then free
// the pInfo block of every 0x2C-byte record whose +0x28 word is non-zero (Fog free, MonsterTbls.cpp line 0x643), free the
// record array (line 0x647), and clear the pointer and the count. Frees are host callbacks.
#include <cstdint>

struct D2Common534B0Tables {
    uintptr_t pLinker;     // DAT_6FDF0890: linker, freed by the 1-argument free 0x9246
    uintptr_t pRecs;       // DAT_6FDF0894: 0x2C-byte records
    int32_t   nRecs;       // DAT_6FDF0898
};
typedef void (*D2Common534B0Free)(uintptr_t p, int32_t nFile, int32_t nLine);   // Fog free 0x9228 (nFile 2 = MonsterTbls.cpp)
typedef void (*D2Common534B0LinkerFree)(uintptr_t p);                           // 0x9246

// 1.13c D2Common+0x534B0
extern "C" void D2Common_534B0_Release(D2Common534B0Tables* t, D2Common534B0Free pFree, D2Common534B0LinkerFree pLinkerFree)
{
    pLinkerFree(t->pLinker);
    for (int32_t i = 0; i < t->nRecs; ++i) {
        const uint32_t* rec = reinterpret_cast<const uint32_t*>(t->pRecs + (uintptr_t)i * 0x2C);
        if (rec[0x28 / 4] != 0) pFree(rec[0x28 / 4], 2, 0x643);
    }
    pFree(t->pRecs, 2, 0x647);
    t->pRecs = 0;
    t->nRecs = 0;
}
