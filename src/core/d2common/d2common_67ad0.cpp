// Path planner with a fallback search, reached through a pointer table. ECX = path info (+0 start point as two words x,y; +4 target;
// +0x14 byte tolerance; +0x30 the dynamic block). The block's +0x24 and +0x28 are cleared and the primary planner (0x678B0, ECX = info)
// runs; its result k indexes the block's point list (+0x98 + 4*k). When k != 0 and the point list entry is within the tolerance of the
// target (0x3E2B0, stdcall: entry, info+4, compared against the tolerance byte) and the entry's low word equals the start's x and
// high word its y, the planner result k is returned. Otherwise the squared distance between start (+0) and target (+4) is formed from
// word differences; at 0x145 or more k is returned. Under it the A* search (0x7B630, ECX = info) runs: a non-zero result is returned;
// zero with k != 0 reruns the primary planner and returns its result; zero with k == 0 returns 0.
#include <cstdint>
#include <cstring>

struct D2PathPlanSeam {
    uint32_t (*pfnPrimary)(uint32_t pPathInfo);
    int32_t (*pfnDistance)(uint32_t nPointA, uint32_t nPointB);
    uint32_t (*pfnSearch)(uint32_t pPathInfo);
};

namespace {
uint32_t R32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
void W32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
}

// 1.13c D2Common+0x67AD0
extern "C" uint32_t D2Common_67AD0_PlanPath(const D2PathPlanSeam* s, uint32_t pPathInfo)
{
    const uint32_t pDyn = R32(pPathInfo + 0x30);
    W32(pDyn + 0x24, 0);
    W32(pDyn + 0x28, 0);
    const uint32_t k = s->pfnPrimary(pPathInfo);
    if (k != 0) {
        const uint32_t nPoint = R32(pDyn + 0x98 + k * 4);
        const int32_t nDist = s->pfnDistance(nPoint, R32(pPathInfo + 4));
        if (nDist <= (int32_t)(R32(pPathInfo + 0x14) & 0xff)) {
            if ((uint16_t)nPoint != (uint16_t)R32(pPathInfo)) return k;
            if ((uint16_t)(nPoint >> 16) != (uint16_t)(R32(pPathInfo) >> 16)) return k;
        }
    }
    const uint32_t nStart = R32(pPathInfo), nTarget = R32(pPathInfo + 4);
    int32_t dx = (int32_t)(nStart & 0xffff) - (int32_t)(nTarget & 0xffff);
    if (dx < 0) dx = -dx;
    int32_t dy = (int32_t)(nStart >> 16) - (int32_t)(nTarget >> 16);
    if (dy < 0) dy = -dy;
    if (dx * dx + dy * dy > 0x144) return k;
    const uint32_t r = s->pfnSearch(pPathInfo);
    if (r != 0) return r;
    if (k == 0) return 0;
    return s->pfnPrimary(pPathInfo);
}
