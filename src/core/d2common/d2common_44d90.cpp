// 1.13c D2Common+0x44d90
// Four palette bytes (+0x88..+0x8b) of the txt row named by pUnit->pInfo+0x1d0.
#include <cstdint>
#include <cstring>
#include "D2DataTbls.h"
#include "DataTbls/LevelsTbls.h"

namespace {
uint32_t Rd32u(uint32_t a, uint32_t off) { uint32_t v; std::memcpy(&v, reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)) + off, 4); return v; }
uint32_t Rd32(const uint8_t* p, uint32_t off) { uint32_t v; std::memcpy(&v, p + off, 4); return v; }
const uint8_t* Ptr(const uint8_t* p, uint32_t off) { return reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(Rd32(p, off))); }
}

// pDataTables = the sgptDataTables block (count at +0xC5C), pRows = the 0x9C-byte txt rows (DAT_6fdf0b38).
// The engine passes its own globals; the FN probe passes its pinned copies.
extern "C" void D2Common_44D90(const uint8_t* pUnit, const uint8_t* pDataTables, const uint8_t* pRows,
                               uint8_t* pOut0, uint8_t* pOut1, uint8_t* pOut2, uint8_t* pOut3) {
    const int32_t nId = (int32_t)Rd32(Ptr(pUnit, 0x58), 0x1D0);
    const int32_t nCount = (int32_t)Rd32(pDataTables, 0xC5C);
    static const uint8_t kZero[4] = {0, 0, 0, 0};
    // 1.13c: an out-of-range id reads absolute 0x88 (a null-page read, not reproduced); the port writes zeros.
    const uint8_t* pSrc = (nId >= 0 && nId < nCount) ? pRows + (uint32_t)nId * 0x9C + 0x88 : kZero;
    *pOut0 = pSrc[0];
    *pOut1 = pSrc[1];
    *pOut2 = pSrc[2];
    *pOut3 = pSrc[3];
}

// Engine form (native tables): the same rule read from sgptDataTables; the unit is a retail 32-bit token.
// 1.13c D2Common+0x44D90: copy a unit's level palette. The level id is at pGfxDataCopy(+0x58)+0x1D0;
// an id outside [0, nLevelsTxtRecordCount) makes retail read the zero-filled low image at 0x88, so the
// outputs are zero. Otherwise the four bytes come from the levels.bin row's nIntensity/nRed/nGreen/
// nBlue. The unit is the register argument (EAX) and the four outputs are the stack arguments. Retail
// dereferences the unit and its gfx data without null guards, so invalid pointers trap rather than
// silently selecting level zero.
extern "C" void D2Common_44D90_Engine(uint32_t pUnit, uint8_t* pnIntensity, uint8_t* pnRed,
                                               uint8_t* pnGreen, uint8_t* pnBlue)
{
	if (pUnit == 0)
		__builtin_trap();

	const uint32_t pGfx = Rd32u(pUnit, 0x58);
	if (pGfx == 0)
		__builtin_trap();

	const int32_t nLevelId = static_cast<int32_t>(Rd32u(pGfx, 0x1D0));

	uint8_t nIntensity = 0;
	uint8_t nRed = 0;
	uint8_t nGreen = 0;
	uint8_t nBlue = 0;

	if (nLevelId >= 0 && nLevelId < sgptDataTables->nLevelsTxtRecordCount)
	{
		const D2LevelDefBin* pRow = &sgptDataTables->pLevelDefBin[nLevelId];
		nIntensity = pRow->nIntensity;
		nRed = pRow->nRed;
		nGreen = pRow->nGreen;
		nBlue = pRow->nBlue;
	}

	*pnIntensity = nIntensity;
	*pnRed = nRed;
	*pnGreen = nGreen;
	*pnBlue = nBlue;
}
