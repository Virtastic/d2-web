// Scan a room's neighbour list for the first neighbour other than the room itself whose hit test accepts the point;
// the hit result is returned and the neighbour written to the out slot (0 and a cleared slot when none accepts).
#include <cstdint>

struct D2NeighbourHitSeam {
    // the per-room hit test: the room, the caller's flag word (passed through untouched), then the three query words
    int32_t (*pfnHit)(uint32_t pRoom, uint32_t nFlags, int32_t a, int32_t b, int32_t c);
};

// 1.13c D2Common+0x68780
int32_t D2Common_68780_NeighbourHit(const D2NeighbourHitSeam* s, uint32_t nFlags, uint32_t pSelf, int32_t nCount,
                                    const uint32_t* pList, int32_t a, int32_t b, int32_t c, uint32_t* pOut)
{
    for (int32_t i = 0; i < nCount; ++i) {
        if (pList[i] == pSelf) continue;
        const int32_t r = s->pfnHit(pList[i], nFlags, a, b, c);
        if (r != 0) { *pOut = pList[i]; return r; }
    }
    *pOut = 0;
    return 0;
}
