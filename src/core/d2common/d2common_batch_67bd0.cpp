// D2Common batch26: twelve UNMAPPED rows translated from the pinned 1.13c decompiles.
// Opaque retail images (units, rooms, drlg) are reached through their 1.13c 32-bit layout where the
// vendored 1.10f structure has no equivalent field; named fields are used where the layouts agree.
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Inventory.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "DataTbls/BeltsTbls.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/SkillsTbls.h"
#include "Units/MissileStream.h"
#include "Units/Units.h"

// Implemented in d2common_batch_663b0.cpp; called by 0x25BD0.
bool D2Common_24D00_ItemIsBaseVariantNotQuest(uint32_t nItemId);
// Implemented in d2common_batch_1fb80.cpp; called by 0x51C60.
D2UnitStrc* D2Common_20320_GetLeftHandWeapon(D2InventoryStrc* pInventory);
// Implemented in d2common_batch_33020.cpp / d2common_batch_0a090.cpp; called by 0x51C60.
short D2Common_10213_ITEMS_GetStrengthBonus(D2UnitStrc* pItem);
int16_t D2Common_22970_ItemDexBonus(D2UnitStrc* pUnit);
extern "C" {
__attribute__((weak)) intptr_t sub_1E1B0(const char* env);
__attribute__((weak)) char DAT_6fded378[1];
}

namespace
{
	// 0x38A80 / 0x38B70 are the 1.13c stat-total pair (still UNMAPPED). 0x38B70 bounds the id by the
	// itemstatcost count, takes itemstatcost[id] and sums it into the unit's stat-list (EBX in
	// retail). The vendored STATLIST_GetTotalStat_6FDB63E0 is that body; the packed id is the id.
	int32_t Batch26_GetTotalStat(D2UnitStrc* pUnit, int32_t nStatId)
	{
		if (!pUnit || !pUnit->pStatListEx) return 0;
		if (nStatId < 0 || nStatId >= sgptDataTables->nItemStatCostTxtRecordCount) return 0;
		if (!sgptDataTables->pItemStatCostTxt) return 0;
		return STATLIST_GetTotalStat_6FDB63E0(pUnit->pStatListEx,
			D2SLayerStatIdStrc::MakeFromStatId((uint16_t)nStatId).nPackedValue,
			&sgptDataTables->pItemStatCostTxt[nStatId]);
	}
}

// 1.13c D2Common+0x67BD0 (#10309) allocates the 16-byte waypoint record used by the default map
// state: zeroed, first word 0x102, then bit 0 set through the waypoint bit table (word 1, mask 1).
// Retail stores the table as (word index, mask) pairs at the pair-0 globals; 0x67C90's spec pins
// the mapping to word 1 + n/16 and mask 1 << n%16, so pair 0 is {1, 1}.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2common_67bd0(void)
{
	uint8_t* pRecord = (uint8_t*)D2_CALLOC(0x10);
	*(uint16_t*)pRecord = 0x102;
	*(uint16_t*)(pRecord + 1 * 2) |= 1;
	return pRecord;
}

// 1.13c D2Common+0x3BD60 (#10310) reads a pointer out of the object at +0x10/+0x64 and returns its
// +0x30 dword. A null inner pointer is a fatal report in retail (message 0x2CD); we return 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_3bd60(const uint8_t* pObject)
{
	if (!pObject) return 0;
	const uint8_t* pInner = *(const uint8_t* const*)(pObject + 0x10);
	if (!pInner) return 0;
	const uint8_t* pRecord = *(const uint8_t* const*)(pInner + 0x64);
	if (!pRecord) return 0;
	return *(const uint32_t*)(pRecord + 0x30);
}

// 1.13c D2Common+0x4F0E0 (#10313) skills.txt nPassiveState (+0x94) when it is positive; a bad id or
// a non-positive value answers 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_4f0e0(int32_t nSkillId)
{
	if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount) return 0;
	if (!sgptDataTables->pSkillsTxt) return 0;

	const int16_t nPassiveState = sgptDataTables->pSkillsTxt[nSkillId].nPassiveState;
	return nPassiveState > 0 ? nPassiveState : 0;
}

// 1.13c D2Common+0x121D0 (sub_121D0): does the room's subtile box lie within the search margin m around
// (x, y)? 1.13c reads the box from the room's coordinate block (0x8BA60 copies 8 dwords from room +0x4C;
// the first four are subtile x, y, width, height), which the vendored D2ActiveRoomStrc holds as tCoords.
// The 1.13c register contract is ecx = room, edx = x, eax = y, edi = margin.
static bool Batch26_RoomInSearchBox(const D2ActiveRoomStrc* pRoom, int32_t nX, int32_t nY, int32_t nMargin)
{
	const int32_t c0 = pRoom->tCoords.nSubtileX;
	const int32_t c1 = pRoom->tCoords.nSubtileY;
	const int32_t c2 = pRoom->tCoords.nSubtileWidth;
	const int32_t c3 = pRoom->tCoords.nSubtileHeight;
	const bool bYOk = (nY + nMargin >= c1) || (nY - nMargin <= c3 + c1);
	if (nX + nMargin < c0)
		return (nX - nMargin <= c2 + c0) && bYOk;
	return bYOk;
}

// 1.13c D2Common+0x12330 (#10314) walks the rooms near the unit's room, keeps those that pass 0x121D0
// (margin = range, or 0x10000 when range is 0), then walks each one's unit list and keeps the nearest unit
// (0x7F5E0 distance) below both the range and the best so far that the callback accepts. The callback is
// fastcall in retail: ecx = candidate unit, edx = the source unit (the first argument), so it is modelled
// as (candidate, source). A null room is fatal in retail (message 0x90) and a null callback is fatal too
// (message 0x9F); both return null here. Layout delta: the 1.13c near-room array (+0x00, count +0x24) and
// the unit list head (+0x74) are the vendored ppRoomList/nNumRooms and pUnitFirst; units chain by
// pRoomNext (+0xE8 in both).
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2common_12330(D2UnitStrc* pUnit, int32_t nX, int32_t nY,
	int32_t nRange, int32_t (*pfnFilter)(D2UnitStrc*, D2UnitStrc*))
{
	if (!pUnit || !pfnFilter) return nullptr;

	D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
	if (!pRoom) return nullptr;

	const int32_t nThreshold = nRange == 0 ? 0x10000 : nRange;

	void* pBest = nullptr;
	int32_t nBestDistance = 0xFFFF;

	for (int32_t i = 0; i < pRoom->nNumRooms; ++i)
	{
		D2ActiveRoomStrc* pNear = pRoom->ppRoomList[i];
		if (!pNear || !Batch26_RoomInSearchBox(pNear, nX, nY, nThreshold)) continue;

		for (D2UnitStrc* p = pNear->pUnitFirst; p; p = p->pRoomNext)
		{
			const int32_t nDistance = (int32_t)UNITS_GetDistanceToCoordinates(p, nX, nY);
			if (nDistance < nThreshold && nDistance < nBestDistance && pfnFilter(p, pUnit) != 0)
			{
				nBestDistance = nDistance;
				pBest = p;
			}
		}
	}

	return pBest;
}

// 1.13c D2Common+0x2E3E0 (#10316) seeds the 0x48C-byte DRLG level record: zeroed, the parent seed
// at +0x00 and its LCG state at +0x04, the caller's parameters, and the per-difficulty seed draws
// (difficulty 1 draws two distinct 0..6 values into +0x94/+0x484 and re-stores the LCG state;
// difficulty 2 draws once into the state and keeps bit 0 at +0x474).
// partial: the 0x2D4E0/0x7B8D0 initialize calls, the per-difficulty generators (0x17930..0x17AE0),
// the optional level allocation through 0x2D9B0/0x2D9B0 and the +0x478 parent write (a register
// argument the decompiler loses) are not translated.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_fn_d2common_2e3e0(const uint8_t* pDrlg, uint8_t nDifficulty,
	uint32_t nParam3, uint32_t nParam4, uint8_t nParam5, uint32_t nParam6, uint32_t nParam7)
{
	uint8_t* pLevel = (uint8_t*)D2_CALLOC(0x48C);

	const uint32_t nSeed = pDrlg ? *(const uint32_t*)(pDrlg + 0x5C) : 0;
	const uint64_t nState = (uint64_t)nSeed * 0x6AC690C5ull + 0x29Aull;

	*(uint32_t*)(pLevel + 0x8C) = nParam3;
	*(uint32_t*)(pLevel + 0x9C) = nParam4;
	*(uint8_t*)(pLevel + 0x450) = nParam5;
	*(uint32_t*)(pLevel + 0x454) = nParam6;
	*(uint32_t*)(pLevel + 0x458) = nSeed;
	*(uint32_t*)(pLevel + 0x464) = nDifficulty;
	*(uint64_t*)(pLevel + 0x000) = nState;
	*(char*)(pLevel + 0x480) = (char)nDifficulty;
	*(uint32_t*)(pLevel + 0x46C) = (uint32_t)(uintptr_t)pDrlg;
	*(uint32_t*)(pLevel + 0x470) = (uint32_t)nState;
	*(uint32_t*)(pLevel + 0x488) = nParam7;
	*(uint32_t*)(pLevel + 0x004) = 0x29A;

	if (nDifficulty == 1)
	{
		uint64_t nDraw;
		int32_t nFirst;
		int32_t nSecond;

		do
		{
			nDraw = (nState & 0xFFFFFFFFull) * 0x6AC690C5ull + (nState >> 32);
			nFirst = (int32_t)((nDraw & 0xFFFFFFFFull) % 7);
			nDraw = (nDraw & 0xFFFFFFFFull) * 0x6AC690C5ull + (nDraw >> 32);
			nSecond = (int32_t)((nDraw & 0xFFFFFFFFull) % 7);
		} while (nFirst == nSecond);

		*(uint32_t*)(pLevel + 0x94) = nFirst + 0x42;
		*(uint64_t*)(pLevel + 0x000) = nDraw;
		*(uint32_t*)(pLevel + 0x484) = nSecond + 0x42;
	}
	else if (nDifficulty == 2)
	{
		const uint64_t nDraw = (nState & 0xFFFFFFFFull) * 0x6AC690C5ull + (nState >> 32);
		*(uint64_t*)(pLevel + 0x000) = nDraw;
		*(uint32_t*)(pLevel + 0x474) = (uint32_t)nDraw & 1;
	}

	return pLevel;
}

// 1.13c D2Common+0x22F50 (#10321) items.txt wMissileType (+0xFA) of an item unit; any other unit
// type answers 0xFFFFFFFF. A missing items row is fatal in retail; we answer 0xFFFFFFFF.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_22f50(D2UnitStrc* pUnit)
{
	if (!pUnit || pUnit->dwUnitType != UNIT_ITEM) return 0xFFFFFFFF;

	const D2ItemsTxt* pItem = DATATBLS_GetItemsTxtRecord(pUnit->dwClassId);
	return pItem ? pItem->wMissileType : 0xFFFFFFFF;
}

// 1.13c D2Common+0x51C60 (#10323) item damage/speed contribution: adds the itemstatcost row 137
// total to both accumulators, merges the left-hand weapon's stat list into the unit, then folds the
// item's min/max damage, strength/dexterity percentages, stat 0x19 and stat 0x11 into the outputs.
// ApplyRatio's divisor (register in retail) is taken as 100, as at its other callers.
namespace
{
	bool Batch26_ShouldMerge(D2UnitStrc* pItem)
	{
		if (!pItem) return false;
		if (!ITEMS_CheckItemTypeId(pItem, 0x2D)) return false;
		if (!ITEMS_CanBeEquipped(pItem)) return false;

		D2StatListExStrc* pStatListEx = pItem->pStatListEx;
		if (!pStatListEx) return false;

		D2StatListStrc* pList = pStatListEx->pParent;
		if (!pList || (int32_t)pList->dwFlags >= 0) return false;

		D2StatListExStrc* pChild = STATLIST_StatListExCast(pList);
		return pChild != nullptr && pChild->pOwner != nullptr;
	}
}

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_51c60(D2UnitStrc* pUnit, int32_t* pMinOut,
	int32_t* pMaxOut, int32_t* pBonusOut)
{
	if (!pUnit) return;

	int32_t nBase = 0;
	if (pUnit->pStatListEx &&
		sgptDataTables->nItemStatCostTxtRecordCount >= 0x8A &&
		sgptDataTables->pItemStatCostTxt != nullptr)
	{
		nBase = STATLIST_GetTotalStat_6FDB63E0(pUnit->pStatListEx,
			D2SLayerStatIdStrc::MakeFromStatId(137).nPackedValue,
			&sgptDataTables->pItemStatCostTxt[137]);
	}

	*pMinOut += nBase;
	*pMaxOut += nBase;

	D2InventoryStrc* pInventory = pUnit->pInventory;
	if (!pInventory) return;

	D2UnitStrc* pLeft = D2Common_20320_GetLeftHandWeapon(pInventory);
	D2UnitStrc* pOther = nullptr;

	if (pLeft)
	{
		uint8_t nBodyLoc = 0;
		if (pLeft->dwUnitType == UNIT_ITEM && pLeft->pItemData)
		{
			nBodyLoc = pLeft->pItemData->nBodyLoc;
		}
		pOther = INVENTORY_GetItemFromBodyLoc(pInventory, (nBodyLoc == 4) + 4);

		if (Batch26_ShouldMerge(pLeft))
		{
			STATLIST_MergeStatLists(pUnit, pLeft, 0);
		}
		else
		{
			pLeft = nullptr;
		}

		if (pOther && pOther != pLeft && Batch26_ShouldMerge(pOther))
		{
			STATLIST_MergeStatLists(pUnit, pOther, 0);
		}
	}

	// Inventory item block: signature 0x01020304, table record from 0x1E1B0(&DAT_6fded378), item at
	// (record+0xC)->+0x24.
	if (*(uint32_t*)pInventory == 0x01020304 && sub_1E1B0 && sub_1E1B0(DAT_6fded378))
	{
		const intptr_t e = sub_1E1B0(DAT_6fded378);
		D2UnitStrc* pEq = *(D2UnitStrc**)(*(intptr_t*)(e + 0xC) + 0x24);
		const D2ItemsTxt* pTxt = (pEq && pEq->dwUnitType == UNIT_ITEM) ? DATATBLS_GetItemsTxtRecord(pEq->dwClassId) : nullptr;
		if (pTxt)
		{
			int32_t nMin = pTxt->nMinDam + nBase;
			int32_t nMax = pTxt->nMaxDam + nBase;
			int32_t nPct = 0;
			if (D2Common_10213_ITEMS_GetStrengthBonus(pEq))
				nPct = DATATBLS_ApplyRatio(100, Batch26_GetTotalStat(pUnit, 0), 100);
			if (D2Common_22970_ItemDexBonus(pEq))
				nPct += DATATBLS_ApplyRatio(100, Batch26_GetTotalStat(pUnit, 2), 100);
			nPct += Batch26_GetTotalStat(pUnit, 0x19);
			if (nPct < -0x59) nPct = -0x5A;
			const int32_t nStat11 = Batch26_GetTotalStat(pUnit, 0x11);
			if (nMax <= nMin) nMax = nMin;
			*pBonusOut += nStat11 + nPct;
			*pMinOut += nMin;
			*pMaxOut += nMax;
		}
	}

	if (pLeft)
	{
		STATLIST_MergeStatLists(pUnit, pLeft, 1);
	}
}

// 1.13c D2Common+0x4E0A0 (#10325) stores the dword at +0x14. A null object is fatal in retail
// (message 0xA3F); here it is a no-op.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_4e0a0(uint8_t* pObject, uint32_t nValue)
{
	if (!pObject) return;
	*(uint32_t*)(pObject + 0x14) = nValue;
}

// 1.13c D2Common+0x24BF0 (#10326) items.txt n2Handed (+0x11C) of an item unit; any other unit type
// answers 0. The items row is reached through 0x22E70, whose stride is 0x1A8 and whose out-of-range
// path is a fatal report in retail; we answer 0.
// 1.13c D2Common+0x24BF0
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t d2_fn_d2common_24bf0(D2UnitStrc* pUnit)
{
	if (!pUnit || pUnit->dwUnitType != UNIT_ITEM) return 0;

	const D2ItemsTxt* pItem = DATATBLS_GetItemsTxtRecord(pUnit->dwClassId);
	return pItem ? pItem->n2Handed : 0;
}

// 1.13c D2Common+0x25BD0 (#10328) applies the base-variant check (0x24D00) to an item unit's class
// id. A null unit (0x78D) or a non-item (0x78E) is fatal in retail; here a no-op.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_25bd0(D2UnitStrc* pUnit)
{
	if (!pUnit || pUnit->dwUnitType != UNIT_ITEM) return;

	(void)D2Common_24D00_ItemIsBaseVariantNotQuest((uint32_t)pUnit->dwClassId);
}

// 1.13c D2Common+0x1D760 (#10330) installs an empty two-dword missile stream (units/MisStream.cpp)
// on a missile whose data block has no stream. A non-missile, a missing data block or an existing
// stream is left untouched; the allocation still happens in retail before the ownership check.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_1d760(D2UnitStrc* pUnit)
{
	const bool bMissile = pUnit && pUnit->dwUnitType == UNIT_MISSILE && pUnit->pMissileData;
	const bool bHasStream = bMissile && pUnit->pMissileData->pStream != nullptr;
	if (bHasStream) return;

	void* pStream = D2_CALLOC(8);
	if (bMissile)
	{
		pUnit->pMissileData->pStream = (D2MissileStreamStrc*)pStream;
	}
	(void)pStream;
}

// 1.13c D2Common+0x13340 (#10333) GetGlobalBeltSlotPosition: copies the belt box nBoxId of row
// nHigherRes * 7 + nIndex out of the belts.txt table (record stride 0x108, box stride 0x10, boxes at
// +0x08) into the caller's four dwords. A missing table or row is fatal in retail (0x79/0x7A/0x8F);
// here a no-op.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2common_GetGlobalBeltSlotPosition(int32_t nIndex, int32_t nHigherRes,
	int32_t* pOut, int32_t nBoxId)
{
	static_assert(sizeof(D2BeltsTxt) == 0x108, "belts.txt record stride");
	static_assert(sizeof(D2InvRectStrc) == 0x10, "belts.txt box stride");

	if (!gpBeltsTxtTable || !pOut) return;
	if (nBoxId < 0 || nBoxId >= 16) return;

	const D2BeltsTxt* pRow = &gpBeltsTxtTable[nHigherRes * BELT_IDX_COUNT_PER_RESOLUTION + nIndex];
	pOut[0] = pRow->beltBoxes[nBoxId].nLeft;
	pOut[1] = pRow->beltBoxes[nBoxId].nRight;
	pOut[2] = pRow->beltBoxes[nBoxId].nTop;
	pOut[3] = pRow->beltBoxes[nBoxId].nBottom;
}
