// 1.13c D2Common implementation batch (rows 0x34150, 0x2FFE0, 0x6CA70, 0x4F070, 0x82E80, 0x50CE0,
// 0x23E30, 0x30600, 0x4E000, 0x4D7E0, 0x21EE0, 0x24240).
// Bodies follow the cached 1.13c decompiles (build-wasm/ghidra-d2common/decomp/0x<RVA>.c).
// Callees whose bodies are not recovered are env-seam function pointers after the retail arguments;
// a null seam contributes 0 and the body never faults on it. Fatal paths that end in _exit(-1) are
// modelled as returns, as in the rest of src/core/d2common.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Inventory.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "D2States.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/LevelsTbls.h"
#include "DataTbls/OverlayTbls.h"
#include "Path/Path.h"
#include "Units/Item.h"
#include "Units/Player.h"
#include "Units/UnitRoom.h"
#include "Units/Units.h"

// 0x223D0 and 0x30420 are already NAMED rows in their own files.
extern "C" uint32_t d2common_IsTransmogrify_223d0(int32_t nItemId);
extern "C" void d2_fn_d2common_30420(uint32_t* r);

static_assert(offsetof(D2OverlayTxt, dwOffsetX) == 0x54, "overlay.txt dwOffsetX");
static_assert(sizeof(D2OverlayTxt) == 0x84, "overlay.txt record");
static_assert(offsetof(D2LvlTypesTxt, szFile) == 0x00 && sizeof(D2LvlTypesTxt) == 0x788,
              "lvltypes.txt record");
static_assert(offsetof(D2ItemTypesTxt, nBeltable) == 0x19, "itemtypes.txt nBeltable");
static_assert(offsetof(D2ItemsTxt, nInvWidth) == 0x10F, "items.txt nInvWidth");
static_assert(offsetof(D2ItemsTxt, nInvHeight) == 0x110, "items.txt nInvHeight");
static_assert(sizeof(void*) != 4 || offsetof(D2StaticPathStrc, dwClientCoordX) == 0x04, "static path x");
static_assert(offsetof(D2DynamicPathStrc, dwClientCoordX) == 0x08, "dynamic path x");
static_assert(offsetof(D2ItemDataStrc, dwItemFlags) == 0x18, "item data flags");
static_assert(offsetof(D2ItemDataStrc, nInvPage) == 0x45, "item data inventory page");
static_assert(offsetof(D2ItemDataStrc, pExtraData) == 0x5C, "item data extra data");
static_assert(sizeof(void*) != 4 || offsetof(D2ItemExtraDataStrc, pNextItem) == 0x08, "item data next item");
static_assert(sizeof(void*) != 4 || offsetof(D2ItemExtraDataStrc, nNodePosOther) == 0x0D, "item data node position");
static_assert(sizeof(void*) != 4 || offsetof(D2PlayerDataStrc, nSwitchRightSkillId) == 0x80, "player switch skill id");
static_assert(sizeof(void*) != 4 || offsetof(D2PlayerDataStrc, nSwitchRightSkillFlags) == 0x88,
              "player switch skill flags");

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t R(const uint8_t* b, int32_t o) { return *reinterpret_cast<const uint32_t*>(b + o); }
}

// 1.13c D2Common+0x34150 (#10627): move the state bits selected by a stay-death mask from the first
// bit array of an extended stat list to its second (gfx) array, then refresh the unit's room. The
// mask is the player, monster or boss stay-death mask by unit type and bBoss; a missing stat list,
// a non-extended one or a missing bit array is a no-op. Prior art: D2MOO 1.10f
// STATES_UpdateStayDeathFlags, whose three branches this reproduces.
extern "C"
EMSCRIPTEN_KEEPALIVE void D2Common_UpdateStayDeathFlags(D2UnitStrc* pUnit, int32_t bBoss)
{
	if (!pUnit || !sgptDataTables)
		return;

	D2StatListExStrc* pStatList = pUnit->pStatListEx;
	if (!pStatList || (pStatList->dwFlags & STATLIST_EXTENDED) == 0 || !pStatList->StatFlags)
		return;

	const int32_t nWords = (sgptDataTables->nStatesTxtRecordCount + 0x1F) / 0x20;

	int32_t nMask = STATEMASK_MONSTAYDEATH;
	if (bBoss)
		nMask = STATEMASK_BOSSSTAYDEATH;
	else if (pUnit->dwUnitType == 0)
		nMask = STATEMASK_PLRSTAYDEATH;

	const uint32_t* pMask = sgptDataTables->fStateMasks[nMask];
	if (!pMask)
		return;

	uint32_t* pBits = pStatList->StatFlags;
	uint32_t* pGfx = pBits + nWords;
	for (int32_t i = 0; i < nWords; ++i)
	{
		pGfx[i] |= ~pMask[i] & pBits[i];
		pBits[i] &= pMask[i];
	}

	UNITROOM_RefreshUnit(pUnit);
}

// 1.13c D2Common+0x2FFE0 (#10628): client coordinates of a unit, written as two dwords. Object,
// item and tile units read the static path (x +0x04, y +0x08), everything else the dynamic path
// (x +0x08, y +0x0C). A missing path yields 0/0.
extern "C"
EMSCRIPTEN_KEEPALIVE void D2Common_2FFE0_GetUnitClientCoords(D2UnitStrc* pUnit, uint32_t* pOut)
{
	if (!pOut)
		return;
	pOut[0] = 0;
	pOut[1] = 0;
	if (!pUnit)
		return;

	const uint32_t nType = pUnit->dwUnitType;
	if (nType == 2 || nType == 4 || nType == 5)
	{
		const D2StaticPathStrc* pPath = pUnit->pStaticPath;
		if (!pPath)
			return;
		pOut[0] = static_cast<uint32_t>(pPath->dwClientCoordX);
		pOut[1] = static_cast<uint32_t>(pPath->dwClientCoordY);
	}
	else
	{
		const D2DynamicPathStrc* pPath = pUnit->pDynamicPath;
		if (!pPath)
			return;
		pOut[0] = static_cast<uint32_t>(pPath->dwClientCoordX);
		pOut[1] = static_cast<uint32_t>(pPath->dwClientCoordY);
	}
}

// 1.13c D2Common+0x6CA70 (#10629): copy file name nFile (0..0x1F) of lvltypes.txt record nRecord to
// szOut. A missing table or record index leaves szOut empty; the source field is 60 bytes.
extern "C"
EMSCRIPTEN_KEEPALIVE void D2COMMON_GetLvlTypeFile(int32_t nRecord, int32_t nFile, char* szOut)
{
	if (!szOut)
		return;
	szOut[0] = '\0';
	if (!sgptDataTables || !sgptDataTables->pLvlTypesTxt)
		return;
	if (nRecord < 0 || nRecord >= sgptDataTables->nLvlTypesTxtRecordCount)
		return;
	if (nFile < 0 || nFile >= 0x20)
		return;

	const char* szSrc = sgptDataTables->pLvlTypesTxt[nRecord].szFile[nFile];
	int32_t i = 0;
	for (; i < 59 && szSrc[i]; ++i)
		szOut[i] = szSrc[i];
	szOut[i] = '\0';
}

// 1.13c D2Common+0x4F070 (#10630): transparent forwarder to 0x4E480, a register-argument thunk.
// From the list at pOwner+0xA8 it keeps the node whose first pointer's int16 equals nMatch, whose
// +0x3C word is 0, and that wins the ordering: no candidate yet, its +0x34 is -1, or the current
// best's +0x34 is not -1 and its +0x28 is lower. Returns the chosen node, 0 when none. 0x4E480 is
// still an UNMAPPED row; its loop is reproduced here with the thunk's own registers as arguments.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_FindSkillNodeById_4F070(uint32_t pOwner, int32_t nMatch)
{
	if (!pOwner)
		return 0;
	const uint32_t pList = R(P(pOwner), 0xA8);
	if (!pList)
		return 0;

	uint32_t pBest = 0;
	for (uint32_t pNode = R(P(pList), 0x04); pNode; pNode = R(P(pNode), 0x04))
	{
		const uint8_t* node = P(pNode);
		const uint32_t pFirst = R(node, 0x00);
		const int16_t nKey = pFirst ? *reinterpret_cast<const int16_t*>(P(pFirst)) : 0;
		if (nKey != nMatch || R(node, 0x3C) != 0)
			continue;

		bool bBetter;
		if (!pBest)
			bBetter = true;
		else if (static_cast<int32_t>(R(node, 0x34)) == -1)
			bBetter = true;
		else
			bBetter = static_cast<int32_t>(R(P(pBest), 0x34)) != -1 &&
			          static_cast<int32_t>(R(P(pBest), 0x28)) < static_cast<int32_t>(R(node, 0x28));
		if (bBetter)
			pBest = pNode;
	}
	return pBest;
}

// 1.13c D2Common+0x82E80 (#10632): overlay.txt record nOverlay's dwOffsetX (+0x54). An index
// outside the table or a missing table yields 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2COMMON_GetOverlayOffsetX(int32_t nOverlay)
{
	if (!sgptDataTables || !sgptDataTables->pOverlayTxt)
		return 0;
	if (nOverlay < 0 || nOverlay >= sgptDataTables->nOverlayTxtRecordCount)
		return 0;
	return sgptDataTables->pOverlayTxt[nOverlay].dwOffsetX;
}

// 0x50CE0's callees 0x81B90 and 0x767F0 are still UNMAPPED rows (no cached body).
struct D2Common50CE0Env
{
	uint32_t (*pGetList)(uint32_t pUnit, uint32_t nArg);
	int32_t (*pGetDivisor)(uint32_t pUnit, uint32_t pList);
};

// 1.13c D2Common+0x50CE0 (#10639): for a player unit whose dwFlagEx (+0xC8) has bit 3 set, divide
// the low byte-cleared dwFrameCountPrecise (+0x48) by the divisor from 0x767F0 (0x13 when 0x81B90
// answers null, 0 when the divisor is not positive).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_50ce0(D2UnitStrc* pUnit, const D2Common50CE0Env* pEnv)
{
	if (!pUnit || pUnit->dwUnitType != 0)
		return 0;
	if (((pUnit->dwFlagEx >> 3) & 1) == 0)
		return 0;

	int32_t nDivisor = 0x13;
	const uint32_t pList = (pEnv && pEnv->pGetList) ? pEnv->pGetList(static_cast<uint32_t>(
	                                                      reinterpret_cast<uintptr_t>(pUnit)), 0)
	                                                : 0;
	if (pList)
	{
		nDivisor = (pEnv && pEnv->pGetDivisor)
		               ? pEnv->pGetDivisor(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pUnit)),
		                                   pList)
		               : 0;
		if (nDivisor < 1)
			return 0;
	}

	const int64_t nValue = static_cast<int64_t>(pUnit->dwFrameCountPrecise) &
	                       static_cast<int64_t>(~static_cast<int64_t>(0xFF));
	return static_cast<int32_t>(nValue / nDivisor);
}

// 1.13c D2Common+0x23E30 (#10640): count the items on a unit's inventory list (signature
// 0x01020304) that sit on inventory page 0 or 3 at node position 1, or at node position 3. Items
// flagged 0x10 in their item data are skipped. The walk follows the item data's pNextItem (+0x64)
// and stops on a null or sentinel (0xFFFFFFA4) data pointer.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2common_CountItemsByNodePos_23e30(D2UnitStrc* pUnit)
{
	int32_t nCount = 0;
	if (!pUnit)
		return nCount;
	D2InventoryStrc* pInventory = pUnit->pInventory;
	if (!pInventory || pInventory->dwSignature != 0x01020304)
		return nCount;

	D2UnitStrc* pNode = pInventory->pFirstItem;
	while (pNode)
	{
		if (pNode->dwUnitType != 4)
			return nCount;

		D2ItemDataStrc* pItemData = pNode->pItemData;
		if (pItemData && (static_cast<intptr_t>(reinterpret_cast<uintptr_t>(pItemData)) != -0x5C) &&
		    (pItemData->dwItemFlags & 0x10) == 0)
		{
			const uint8_t nPos = pItemData->pExtraData.nNodePosOther;
			if (nPos == 1)
			{
				if (pItemData->nInvPage == 0 || pItemData->nInvPage == 3)
					++nCount;
			}
			else if (nPos == 3)
			{
				++nCount;
			}
		}

		if (!pItemData)
			return nCount;
		if (static_cast<intptr_t>(reinterpret_cast<uintptr_t>(pItemData)) == -0x5C)
			return nCount;
		pNode = pItemData->pExtraData.pNextItem;
	}
	return nCount;
}

// 1.13c D2Common+0x30600 (#10645): copy the switch-right skill id (int16 at the record 0x30420
// returns) and that record's +0x34 word into the player data at +0x80/+0x88. Null unit, a non-player
// unit or missing player data is a no-op; a null record writes the id 0 (retail then exits).
extern "C"
EMSCRIPTEN_KEEPALIVE void D2Common_SetPlayerSwitchRightSkill(D2UnitStrc* pUnit)
{
	if (!pUnit || pUnit->dwUnitType != 0)
		return;
	D2PlayerDataStrc* pPlayerData = pUnit->pPlayerData;
	if (!pPlayerData)
		return;

	uint32_t r[5] = { static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pUnit)), 0, 0, 0, 0 };
	d2_fn_d2common_30420(r);
	const uint32_t pRecord = r[4];
	if (!pRecord)
	{
		pPlayerData->nSwitchRightSkillId = 0;
		return;
	}

	const uint32_t pSkills = R(P(pRecord), 0x00);
	pPlayerData->nSwitchRightSkillId =
	    pSkills ? static_cast<int32_t>(*reinterpret_cast<const int16_t*>(P(pSkills))) : 0;
	pPlayerData->nSwitchRightSkillFlags = static_cast<int32_t>(R(P(pRecord), 0x34));
}

// 1.13c D2Common+0x4E000 (#10649): the dword at +0x1C of the given block. A null block yields 0
// (retail exits).
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_BlockDword1C(uint32_t pBlock)
{
	if (!pBlock)
		return 0;
	return R(P(pBlock), 0x1C);
}

struct D2Common4D140Env;
extern "C" int* D2Common_4D140_FindFreeSpot(const D2Common4D140Env* e, void* pLevel, int* pPos, uint32_t* pUnit,
	uint32_t p4, uint32_t p5, uint32_t p6, int p7, int p8, int p9);

// 1.13c D2Common+0x4D7E0 (#10654): forwarder to 0x4D140 with p8 = 0x32, p9 = 1.
extern "C"
int* D2Common_4D7E0_FindFreeSpotR50(const D2Common4D140Env* e, void* pLevel, int* pPos, uint32_t* pUnit,
                                    uint32_t p4, uint32_t p5, uint32_t p6, int p7)
{
	return D2Common_4D140_FindFreeSpot(e, pLevel, pPos, pUnit, p4, p5, p6, p7, 0x32, 1);
}

// 0x21D70 is a still-UNMAPPED row.
struct D2Common21EE0Env
{
	int32_t (*pPlaceInBelt)(int32_t nArg, D2UnitStrc* pItem);
};

// 1.13c D2Common+0x21EE0 (#10655): whether a 1x1 item that is belt-able fits the belt slot
// requested through 0x21D70. Non-item units, non-belt-able items and other item sizes yield 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_ItemBeltSlotFits_21ee0(int32_t nArg, D2UnitStrc* pItem,
                                                   const D2Common21EE0Env* pEnv)
{
	if (!pItem || pItem->dwUnitType != 4)
		return 0;
	if (!ITEMS_CheckIfBeltable(pItem))
		return 0;

	const D2ItemsTxt* pItemsTxt = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	if (!pItemsTxt || pItemsTxt->nInvWidth != 1 || pItemsTxt->nInvHeight != 1)
		return 0;

	if (!pEnv || !pEnv->pPlaceInBelt)
		return 0;
	return static_cast<uint32_t>(pEnv->pPlaceInBelt(nArg, pItem));
}

// 1.13c D2Common+0x24240 (#10661): items.txt record for an item unit's class id must report
// nTransmogrify (0x223D0). A null unit or another unit type is a no-op (retail exits).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2common_ItemTransmogrifyAssert_24240(D2UnitStrc* pItem)
{
	if (!pItem || pItem->dwUnitType != 4)
		return;
	d2common_IsTransmogrify_223d0(pItem->dwClassId);
}
