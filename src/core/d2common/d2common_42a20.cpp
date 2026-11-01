// 1.13c D2Common+0x42A20
// Which of a level's eight linked levels is level B: the vis array of level A's id comes from 0x44CE0 (called with the DRLG
// token at level A +0x1B4); the result is 1 << (slot + 4) for the first slot holding level B's id (+0x1D0), else 0.
// EAX = level A, the stack word = level B. `lookup` is the 0x44CE0 routine (token of the DRLG, level id -> int32_t[8]).
#include <cstdint>
#include <cstring>

extern "C" uint32_t D2Common_42A20(const uint8_t* pLevelA, const uint8_t* pLevelB,
                                   const int32_t* (*lookup)(uint32_t pDrlg, int32_t nLevelId))
{
    uint32_t pDrlg; int32_t idA, idB;
    std::memcpy(&pDrlg, pLevelA + 0x1B4, 4);
    std::memcpy(&idA, pLevelA + 0x1D0, 4);
    const int32_t* pIds = lookup(pDrlg, idA);
    std::memcpy(&idB, pLevelB + 0x1D0, 4);
    int32_t i = 0;
    while (i < 8 && pIds[i] != idB) ++i;
    return i == 8 ? 0 : 1u << (i + 4);
}
