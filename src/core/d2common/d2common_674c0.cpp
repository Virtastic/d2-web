// Straight-line path builder: walk from the start point (+0) toward the target (+4) for at most +0x18 steps,
// picking each step's direction from the delta-direction table rotated by the path's base direction
// (dynamic path +0x98) and the three-candidate test; every direction change appends the previous point to
// the dynamic path's point list (+0x9c, count +0x28), and a walk that stops short appends the last point.
// Retail passes the path info in EBX (no stack words). Tables and callees are seams.
#include <cstdint>
#include <cstring>

struct D2PathStraightSeam {
    const int32_t* pDirTab;   // rows of 3 dwords, indexed by the packed-delta direction
    const int32_t* pStep;     // 8 {x,y} dword pairs, low words used
    int32_t (*pfnDeltaDir)(uint32_t nFrom, uint32_t nTo);
    int32_t (*pfnTryDirs)(uint32_t pPathInfo, uint32_t nPoint, int32_t* pDirs, int32_t* pOut);
};

namespace {
uint32_t R32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
void W32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
}

// 1.13c D2Common+0x674C0
extern "C" void D2Common_674C0_BuildStraightPath(const D2PathStraightSeam* s, uint32_t pPathInfo)
{
    const int32_t nMax = (int32_t)R32(pPathInfo + 0x18);
    const uint32_t pDyn = R32(pPathInfo + 0x30);
    int32_t n = (int32_t)R32(pDyn + 0x28);
    const uint32_t nTarget = R32(pPathInfo + 4);
    const int32_t nBase = (int32_t)R32(pDyn + 0x98);
    uint32_t nCur = R32(pPathInfo);
    int32_t nLast = 0xff, i = 0;
    bool bChanged = false;
    if (nMax > 0) {
        for (;;) {
            if ((uint16_t)nCur == (uint16_t)nTarget && (uint16_t)(nCur >> 16) == (uint16_t)(nTarget >> 16)) break;
            bChanged = false;
            const int32_t* t = s->pDirTab + 3 * s->pfnDeltaDir(nCur, nTarget);
            int32_t aDirs[3] = { (t[0] + nBase) & 7, (t[1] + nBase) & 7, (t[2] + nBase) & 7 };
            int32_t nDir;
            if (!s->pfnTryDirs(pPathInfo, nCur, aDirs, &nDir) || ((nDir - 4) & 7) == nLast) goto stopped;
            {
                const uint32_t nPrev = nCur;
                nCur = (uint32_t)(uint16_t)((nCur >> 16) + s->pStep[nDir * 2 + 1]) << 16 | (uint16_t)(nCur + s->pStep[nDir * 2]);
                if (nDir != nLast) { W32(pDyn + 0x9c + 4 * n, nPrev); ++n; bChanged = true; }
            }
            nLast = nDir;
            if (++i >= nMax) break;
        }
        if (!bChanged) {
stopped:
            if (i != 0) { W32(pDyn + 0x9c + 4 * n, nCur); ++n; }
        }
    }
    W32(pDyn + 0x28, (uint32_t)n);
}
