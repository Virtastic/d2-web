// Path step: try the three candidate directions in turn from a packed point (x low word, y high word) and
// keep the first whose neighbour cell passes the collision test. Retail passes the path info in EAX
// (stack: point, directions, out; ret 0xc). The step table (8 {x,y} dword pairs, low words used), the
// collision test and the fatal exit are seams.
#include <cstdint>
#include <cstring>

struct D2PathTryDirSeam {
    const int32_t* pStep;   // [dir*2] = dx, [dir*2+1] = dy
    int32_t (*pfnCollide)(uint32_t pRoom, uint32_t x, uint32_t y, uint32_t nPattern, uint32_t nMask);
    void (*pfnFatal)(uint32_t nLine);
};

namespace {
uint32_t R32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
}

// 1.13c D2Common+0x671B0
extern "C" int32_t D2Common_671B0_TryDirections(const D2PathTryDirSeam* s, uint32_t pPathInfo, uint32_t nPoint,
                                                const int32_t* pDirs, int32_t* pOut)
{
    const uint32_t pUnit = R32(R32(pPathInfo + 0x30) + 0x30);
    if (!pUnit || R32(pUnit) > 1) s->pfnFatal(0x34);
    const uint32_t pRoom = R32(pPathInfo + 8), nPattern = R32(pPathInfo + 0x28), nMask = R32(pPathInfo + 0x2c);
    for (int k = 0; k < 3; ++k) {
        if (k == 2 && pDirs[2] == 0xff) s->pfnFatal(0x4c);
        const int32_t d = pDirs[k];
        const uint32_t x = (uint16_t)(s->pStep[d * 2] + nPoint);
        const uint32_t y = (uint16_t)(s->pStep[d * 2 + 1] + (nPoint >> 16));
        if (s->pfnCollide(pRoom, x, y, nPattern, nMask) == 0) { *pOut = d; return 1; }
    }
    return 0;
}
