// 1.13c D2Common implementation batch (rows 0x2D890, 0x31680, 0x81950, 0x4BB30, 0x4BB70, 0x4BBB0),
// translated from the cached 1.13c decompiles (build-wasm/ghidra-d2common/decomp/0x<RVA>.c).
// Where the retail body is line-for-line a vendored 1.10f function, the vendored one is called; the
// rest follow the decompile stores. Retail's fatal _exit paths are modelled as returns, as elsewhere
// in src/core/d2common.
#include <cstddef>
#include <cstdint>

#include "D2DataTbls.h"
#include "D2Items.h"
#include "D2Monsters.h"
#include "D2Collision.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/MonsterIds.h"
#include "Drlg/D2DrlgDrlg.h"
#include "Units/Units.h"


// 1.13c D2Common+0x31680: "monster blocks with its shield" predicate, factored out of the 1.10f
// UNITS_GetBlockRate monster branch. Diablo, DoomKnight1 and DiabloClone always qualify; ACT3HIRE
// never does. Otherwise the monster's component slot 7 code is looked up: a torch (' hct') is
// rejected, anything else is resolved through the item linker and qualifies when its items.txt
// wType[0] is ITEMTYPE_SHIELD. A null or non-monster unit, an empty slot or an unknown code is 0.
extern "C" int32_t d2_fn_d2common_31680_batch(uint32_t pUnit)
{
	static const uint32_t gnRejectedCodes[] = { ' hct' };   // torch: not a shield

	D2UnitStrc* pMonster = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit));

	int32_t nClassId = pMonster ? pMonster->dwClassId : -1;
	if (nClassId < 0 || nClassId >= sgptDataTables->nMonStatsTxtRecordCount)
	{
		nClassId = -1;
	}

	switch (nClassId)
	{
	case MONSTER_DIABLO:
	case MONSTER_DOOMKNIGHT1:
	case MONSTER_DIABLOCLONE:
		return 1;

	case MONSTER_ACT3HIRE:
		return 0;

	default:
		break;
	}

	unsigned int nComponent = 0;
	if (pMonster && pMonster->dwUnitType == UNIT_MONSTER && pMonster->pMonsterData)
	{
		nComponent = pMonster->pMonsterData->nComponent[7];
	}

	const uint32_t dwCode = D2Common_11069(pMonster, 7, nComponent);
	if (dwCode == 0)
	{
		return 0;
	}

	for (uint32_t dwRejected : gnRejectedCodes)
	{
		if (dwCode == dwRejected)
		{
			return 0;
		}
	}

	int nItemId = 0;
	D2ItemsTxt* pItemRecord = DATATBLS_GetItemRecordFromItemCode(dwCode, &nItemId);
	if (pItemRecord && pItemRecord->wType[0] == ITEMTYPE_SHIELD)
	{
		return 1;
	}

	return 0;
}

// 1.13c D2Common+0x81950: tile-cell reader for an outdoor room. The room's grid pointer is at +0x64;
// a room flagged 0x01 holds one flat cell at +0x30. Otherwise subtile coordinates (world / 5) minus
// the room origin index the row-offset table (+0x20 of the grid), and the row start minus the origin
// plus the column indexes the cell array (+0x1C). A null grid is fatal in retail (Fog #10265, code
// 0x29C); modelled as a 0 return. Retail receives the room in a register the decompiler shows as
// unaff_ESI (its caller 0x3C680 drops it), so it is the first argument here.
extern "C" uint32_t d2_fn_d2common_81950(uint32_t pRoom, int32_t nX, int32_t nY)
{
	const uint8_t* pRoomBytes = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(pRoom));
	if (!pRoomBytes)
	{
		return 0;
	}

	const uint8_t* pGrid = *reinterpret_cast<const uint8_t* const*>(pRoomBytes + 0x64);
	if (!pGrid)
	{
		return 0;
	}

	if (pGrid[0] & 1)
	{
		return *reinterpret_cast<const uint32_t*>(pGrid + 0x30);
	}

	const int32_t nOriginX = *reinterpret_cast<const int32_t*>(pRoomBytes + 0x34);
	const int32_t nOriginY = *reinterpret_cast<const int32_t*>(pRoomBytes + 0x38);
	const int32_t* pRowOffsets = *reinterpret_cast<const int32_t* const*>(pGrid + 0x20);
	const int32_t* pCells = *reinterpret_cast<const int32_t* const*>(pGrid + 0x1C);
	const int32_t nColumn = pRowOffsets[nY / 5 - nOriginY] - nOriginX + nX / 5;

	return *reinterpret_cast<const uint32_t*>(&pCells[nColumn]);
}

// 1.13c D2Common+0x4BB30: clear nMask over the 3x3 bounding box centred on (nX, nY) -- the size-3
// and big-unit-pattern footprint. Builds {nX-1, nY-1, nX+1, nY+1} and walks it through
// COLLISION_ResetCollisionMaskForBoundingBoxRecursively (retail 0x4B2F0).
extern "C" void d2_fn_d2common_4bb30(uint32_t pRoom, int32_t nX, int32_t nY, uint16_t nMask)
{
	D2BoundingBoxStrc pBoundingBox = { nX - 1, nY - 1, nX + 1, nY + 1 };

	COLLISION_ResetCollisionMaskForBoundingBoxRecursively(
		reinterpret_cast<D2ActiveRoomStrc*>(static_cast<uintptr_t>(pRoom)), &pBoundingBox, nMask);
}

namespace
{
	// Grid record and layer prefix, as in d2common_batch_321e0.cpp (0x4C940): the cell array sits at +0x20.
	struct D2Grid2D88Hdr
	{
		int32_t nField0;   // 0x00
		int32_t nField1;   // 0x04
		int32_t nField2;   // 0x08
		int32_t nPad[5];   // 0x0C
		void* pCells;      // 0x20
	};
	static_assert(offsetof(D2Grid2D88Hdr, pCells) == 0x20, "grid header pointer at retail offset 0x20");

	// Clears nMask in the 0x1330 grid cell (nX, nY): layer = record +0x20, index = (nY - layer[+4]) *
	// layer[+8] - layer[+0] + nX. A missing record or layer leaves the grid alone, as retail does.
	void ClearGridCell1330(void* (*pfnGrid1330)(void), int32_t nX, int32_t nY, uint16_t nMask)
	{
		if (!pfnGrid1330)
			return;
		auto* pRecord = static_cast<D2Grid2D88Hdr*>(pfnGrid1330());
		auto* pLayer = pRecord ? static_cast<D2Grid2D88Hdr*>(pRecord->pCells) : nullptr;
		if (!pLayer || !pLayer->pCells)
			return;
		const int32_t nIndex = (nY - pLayer->nField1) * pLayer->nField2 - pLayer->nField0 + nX;
		auto* pCell = reinterpret_cast<uint16_t*>(static_cast<uint8_t*>(pLayer->pCells) +
			static_cast<intptr_t>(nIndex) * 2);
		*pCell &= static_cast<uint16_t>(~nMask);
	}
}

// 1.13c D2Common+0x4BB70: clear nMask on the single subtile (nX, nY) of the 0x1330 grid. Retail takes the
// mask in CX and x in EDX, y on the stack; the first argument is not read.
extern "C" void d2_fn_d2common_4bb70(uint32_t nUnused, int32_t nX, int32_t nY, uint16_t nMask,
	void* (*pfnGrid1330)(void))
{
	(void)nUnused;
	ClearGridCell1330(pfnGrid1330, nX, nY, nMask);
}

// D2Common+0x4BBB0 (the plus-shaped reset) now lives in d2common_4bbb0.cpp on the retail signature.
