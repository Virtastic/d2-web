// 1.13c D2Common+0x15980: lay out the five child levels 0x4F..0x53 of a level. Reads the retail layout through
// explicit offsets (32-bit link pointers): the owner DRLG is at level+0x1B4, level width at +0x24, origin at
// +0x1C/+0x20; the DRLG's level list head is +0x47C, next +0x1AC, id +0x1D0; the levels.txt rows are 0x9C bytes
// starting at the levels table, sizeX[difficulty] at row+0xC, sizeY[difficulty] at row+0x18, the difficulty byte
// at DRLG+0x450, the table record count at datatables+0xC5C. A missing level is created by the 0x2D9B0 seam.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd(const uint8_t* b, uint32_t o) { uint32_t v; std::memcpy(&v, b + o, 4); return v; }
inline void wr(uint8_t* b, uint32_t o, uint32_t v) { std::memcpy(b + o, &v, 4); }
inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
}

// 1.13c D2Common+0x15980
extern "C" void D2Common_15980_Run(uint8_t* pLevel, const uint8_t* pDt, const uint8_t* pLevelsTbl,
                                   uint8_t* (*pAlloc)(uint8_t* pDrlg, int32_t nLevelId))
{
    uint8_t* const pDrlg = P(rd(pLevel, 0x1B4));
    const int32_t nWidth = static_cast<int32_t>(rd(pLevel, 0x24));
    int32_t nAccY = 0;
    const uint32_t nDiff = pDrlg[0x450];
    for (int32_t nId = 0x4F; nId < 0x54; ++nId)
    {
        const uint8_t* pRow = (nId < 0 || static_cast<int32_t>(rd(pDt, 0xC5C)) <= nId) ? nullptr : pLevelsTbl + nId * 0x9C;
        const int32_t nSizeX = static_cast<int32_t>(rd(pRow, 0xC + nDiff * 4));
        const int32_t nSizeY = static_cast<int32_t>(rd(pRow, 0x18 + nDiff * 4));
        nAccY -= nSizeY;
        uint8_t* pChild = P(rd(pDrlg, 0x47C));
        while (pChild && static_cast<int32_t>(rd(pChild, 0x1D0)) != nId)
            pChild = P(rd(pChild, 0x1AC));
        if (!pChild)
            pChild = pAlloc(pDrlg, nId);
        wr(pChild, 0x1C, rd(pLevel, 0x1C) + static_cast<uint32_t>(nWidth / 2 - nSizeX / 2));
        wr(pChild, 0x20, rd(pLevel, 0x20) + static_cast<uint32_t>(nAccY));
        wr(pChild, 0x24, static_cast<uint32_t>(nSizeX));
        wr(pChild, 0x28, static_cast<uint32_t>(nSizeY));
    }
}
