// Path-step builder reached through a pointer table. ECX = path info; its +0x30 is the dynamic path block. The block's step counter
// (+0x24) is cleared and its direction base (+0x98) set to -4; the straight walk (0x674C0, path info in EBX) runs once. A zero result
// returns 0. Otherwise the result is stored as the point count (+0x28), the base becomes -2 and the walk runs again; if that adds
// nothing (result minus the first is 0) the base becomes 2 and it runs a third time. The base is restored to -4 and the last
// walk's result, expressed as (difference + first), is returned.
#include <cstdint>
#include <cstring>

struct D2PathStepSeam {
    int32_t (*pfnWalk)(uint32_t pPathInfo);
};

namespace {
uint32_t R32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
void W32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
}

// 1.13c D2Common+0x67850
extern "C" int32_t D2Common_67850_BuildSteps(const D2PathStepSeam* s, uint32_t pPathInfo)
{
    const uint32_t pDyn = R32(pPathInfo + 0x30);
    W32(pDyn + 0x24, 0);
    W32(pDyn + 0x98, 0xfffffffcu);
    const int32_t nFirst = s->pfnWalk(pPathInfo);
    if (nFirst == 0) return 0;
    W32(pDyn + 0x28, (uint32_t)nFirst);
    W32(pDyn + 0x98, 0xfffffffeu);
    int32_t nDiff = (int32_t)((uint32_t)s->pfnWalk(pPathInfo) - (uint32_t)nFirst);
    if (nDiff == 0) {
        W32(pDyn + 0x98, 2);
        nDiff = (int32_t)((uint32_t)s->pfnWalk(pPathInfo) - (uint32_t)nFirst);
    }
    W32(pDyn + 0x98, 0xfffffffcu);
    return (int32_t)((uint32_t)nDiff + (uint32_t)nFirst);
}
