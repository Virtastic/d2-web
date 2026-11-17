#include "D2Inventory.h"
#include "InventoryItemDataGuard.h"

#include <cstddef>

#include "D2Composit.h"
#include "D2DataTbls.h"
#include <DataTbls/MonsterIds.h>
#include "D2Dungeon.h"
#include "D2Items.h"
#include "D2Monsters.h"
#include "D2StatList.h"
#include "Units/UnitRoom.h"
#include "Units/Units.h"
#include <Path/Path.h>

struct D2InventoryComponentItemTypeStrc
{
	int dwCode;
	int nItemType;
};


//D2Common.0x6FDE2310
D2InventoryGridInfoStrc gBodyLocInventoryGridInfo = { 13, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
//D2Common.0x6FDE2328
D2InventoryGridInfoStrc gBeltInventoryGridInfo = { 16, 1, 0, 0, 0, 0, 0, 0, 0, 0 };

//D2Common.0x6FDE2820
D2InventoryComponentItemTypeStrc gComponentItemTypeMap[]
{
	{ 0, ITEMTYPE_NONE_2 },
	{ ' til', ITEMTYPE_NONE_2 },
	{ ' dem', ITEMTYPE_NONE_2 },
	{ ' yvh', ITEMTYPE_NONE_2 },
	{ ' pac', ITEMTYPE_HELM },
	{ ' pks', ITEMTYPE_HELM },
	{ ' mlh', ITEMTYPE_HELM },
	{ ' lhf', ITEMTYPE_HELM },
	{ ' mhg', ITEMTYPE_HELM },
	{ ' nrc', ITEMTYPE_HELM },
	{ ' ksm', ITEMTYPE_HELM },
	{ ' epc', ITEMTYPE_ARMOR },
	{ ' klc', ITEMTYPE_ARMOR },
	{ ' bor', ITEMTYPE_ARMOR },
	{ ' iuq', ITEMTYPE_ARMOR },
	{ ' ael', ITEMTYPE_ARMOR },
	{ ' alh', ITEMTYPE_ARMOR },
	{ ' uts', ITEMTYPE_ARMOR },
	{ ' gnr', ITEMTYPE_ARMOR },
	{ ' lcs', ITEMTYPE_ARMOR },
	{ ' nhc', ITEMTYPE_ARMOR },
	{ ' srb', ITEMTYPE_ARMOR },
	{ ' lps', ITEMTYPE_ARMOR },
	{ ' tlp', ITEMTYPE_ARMOR },
	{ ' dlf', ITEMTYPE_ARMOR },
	{ ' htg', ITEMTYPE_ARMOR },
	{ ' luf', ITEMTYPE_ARMOR },
	{ ' cub', ITEMTYPE_SHIELD },
	{ ' grl', ITEMTYPE_SHIELD },
	{ ' tik', ITEMTYPE_SHIELD },
	{ ' wot', ITEMTYPE_SHIELD },
	{ ' lgl', ITEMTYPE_GLOVES },
	{ ' lgm', ITEMTYPE_GLOVES },
	{ ' lgh', ITEMTYPE_GLOVES },
	{ ' tbl', ITEMTYPE_BOOTS },
	{ ' tbm', ITEMTYPE_BOOTS },
	{ ' tbh', ITEMTYPE_BOOTS },
	{ ' lbl', ITEMTYPE_BELT },
	{ ' lbm', ITEMTYPE_BELT },
	{ ' lbh', ITEMTYPE_BELT },
	{ ' mhb', ITEMTYPE_HELM },
	{ ' hsb', ITEMTYPE_SHIELD },
	{ ' kps', ITEMTYPE_SHIELD },
	{ ' dnw', ITEMTYPE_WAND },
	{ ' nwy', ITEMTYPE_WAND },
	{ ' nwb', ITEMTYPE_WAND },
	{ ' blc', ITEMTYPE_CLUB },
	{ ' xah', ITEMTYPE_THROWING_AXE },
	{ ' dss', ITEMTYPE_SWORD },
	{ ' mcs', ITEMTYPE_SWORD },
	{ ' cam', ITEMTYPE_MACE },
	{ ' exa', ITEMTYPE_AXE },
	{ ' clf', ITEMTYPE_SWORD },
	{ ' dsl', ITEMTYPE_SWORD },
	{ ' alf', ITEMTYPE_MACE },
	{ ' mdh', ITEMTYPE_HAMMER },
	{ ' mhw', ITEMTYPE_HAMMER },
	{ ' src', ITEMTYPE_SWORD },
	{ ' slf', ITEMTYPE_SWORD },
	{ ' lpg', ITEMTYPE_MISSILE_POTION },
	{ ' spg', ITEMTYPE_MISSILE_POTION },
	{ ' lpo', ITEMTYPE_MISSILE_POTION },
	{ ' spo', ITEMTYPE_MISSILE_POTION },
	{ ' rgd', ITEMTYPE_KNIFE },
	{ ' 33d', ITEMTYPE_KNIFE },
	{ ' 33g', ITEMTYPE_KNIFE },
	{ ' rid', ITEMTYPE_KNIFE },
	{ ' vaj', ITEMTYPE_SPEAR },
	{ ' vlg', ITEMTYPE_SPEAR },
	{ ' lip', ITEMTYPE_SPEAR },
	{ ' mlc', ITEMTYPE_SWORD },
	{ ' dsg', ITEMTYPE_SWORD },
	{ ' rps', ITEMTYPE_SPEAR },
	{ ' irt', ITEMTYPE_SPEAR },
	{ ' kip', ITEMTYPE_SPEAR },
	{ ' rps', ITEMTYPE_SPEAR },
	{ ' wbs', ITEMTYPE_BOW },
	{ ' wbl', ITEMTYPE_BOW },
	{ ' bbs', ITEMTYPE_BOW },
	{ ' bbl', ITEMTYPE_BOW },
	{ ' tss', ITEMTYPE_STAFF },
	{ ' tsl', ITEMTYPE_STAFF },
	{ ' tsc', ITEMTYPE_STAFF },
	{ ' tsb', ITEMTYPE_STAFF },
	{ ' xal', ITEMTYPE_AXE },
	{ ' ycs', ITEMTYPE_POLEARM },
	{ ' xig', ITEMTYPE_AXE },
	{ ' xtb', ITEMTYPE_AXE },
	{ ' lah', ITEMTYPE_POLEARM },
	{ ' uam', ITEMTYPE_HAMMER },
	{ ' xap', ITEMTYPE_POLEARM },
	{ ' lah', ITEMTYPE_POLEARM },
	{ ' bxh', ITEMTYPE_CROSSBOW },
	{ ' bxl', ITEMTYPE_CROSSBOW },
	{ ' xat', ITEMTYPE_THROWING_AXE },
	{ ' cps', ITEMTYPE_CLUB },
	{ ' rbs', ITEMTYPE_SWORD },
	{ ' tsm', ITEMTYPE_MACE },
	{ ' pcs', ITEMTYPE_SCEPTER },
	{ ' mpg', ITEMTYPE_MISSILE_POTION },
	{ ' mpo', ITEMTYPE_MISSILE_POTION },
	{ ' fkt', ITEMTYPE_THROWING_KNIFE },
	{ ' dlb', ITEMTYPE_KNIFE },
	{ ' dsb', ITEMTYPE_SWORD },
	{ ' wsb', ITEMTYPE_SWORD },
	{ ' sh2', ITEMTYPE_SWORD },
	{ ' nrb', ITEMTYPE_SPEAR },
	{ ' wbh', ITEMTYPE_BOW },
	{ ' wbc', ITEMTYPE_BOW },
	{ ' bws', ITEMTYPE_BOW },
	{ ' bwl', ITEMTYPE_BOW },
	{ ' tsw', ITEMTYPE_STAFF },
	{ ' xab', ITEMTYPE_AXE },
	{ ' rab', ITEMTYPE_POLEARM },
	{ ' xag', ITEMTYPE_AXE },
	{ ' bxr', ITEMTYPE_CROSSBOW },
	{ ' bxm', ITEMTYPE_CROSSBOW },
	{ ' tbz', ITEMTYPE_NONE_2 },
	{ ' raa', ITEMTYPE_ARMOR },
	{ ' ptl', ITEMTYPE_ARMOR },
	{ ' lms', ITEMTYPE_SHIELD },
	{ ' stg', ITEMTYPE_ARMOR },
	{ ' vqa', ITEMTYPE_BODY_PART },
	{ ' vqc', ITEMTYPE_BODY_PART },
	{ ' lbv', ITEMTYPE_BELT },
	{ ' lbt', ITEMTYPE_BELT },
	{ ' lgv', ITEMTYPE_GLOVES },
	{ ' lgt', ITEMTYPE_GLOVES },
	{ ' tbv', ITEMTYPE_BOOTS },
	{ ' tbt', ITEMTYPE_BOOTS },
	{ ' blf', ITEMTYPE_SWORD },
	{ ' ipm', ITEMTYPE_AXE },
	{ ' lab', ITEMTYPE_THROWING_AXE },
	{ ' gel', ITEMTYPE_CLUB },
	{ ' tlq', ITEMTYPE_ARMOR },
	{ ' xa2', ITEMTYPE_AXE },
	{ ' xaw', ITEMTYPE_AXE },
	{ ' nwg', ITEMTYPE_MACE },
	{ ' amg', ITEMTYPE_MACE },
	{ ' dsw', ITEMTYPE_SWORD },
	{ ' sig', ITEMTYPE_SWORD },
	{ ' dlb', ITEMTYPE_KNIFE },
	{ ' fkb', ITEMTYPE_KNIFE },
	{ ' pss', ITEMTYPE_SPEAR },
	{ ' pst', ITEMTYPE_SPEAR },
	{ ' eps', ITEMTYPE_SPEAR },
	{ ' uov', ITEMTYPE_POLEARM },
	{ ' csw', ITEMTYPE_POLEARM },
	{ ' irk', ITEMTYPE_KNIFE },
	{ ' tps', ITEMTYPE_SPEAR },
	{ ' csg', ITEMTYPE_SCEPTER },
	{ ' psw', ITEMTYPE_SCEPTER },
	{ ' tsh', ITEMTYPE_STAFF },
	{ ' fsm', ITEMTYPE_STAFF },
	{ ' hfh', ITEMTYPE_MACE },
	{ ' ah9', ITEMTYPE_AXE },
	{ ' xa9', ITEMTYPE_AXE },
	{ ' a29', ITEMTYPE_AXE },
	{ ' pm9', ITEMTYPE_AXE },
	{ ' aw9', ITEMTYPE_AXE },
	{ ' al9', ITEMTYPE_AXE },
	{ ' ab9', ITEMTYPE_AXE },
	{ ' tb9', ITEMTYPE_AXE },
	{ ' ag9', ITEMTYPE_AXE },
	{ ' ig9', ITEMTYPE_AXE },
	{ ' nw9', ITEMTYPE_WAND },
	{ ' wy9', ITEMTYPE_WAND },
	{ ' wb9', ITEMTYPE_WAND },
	{ ' wg9', ITEMTYPE_WAND },
	{ ' lc9', ITEMTYPE_MACE },
	{ ' cs9', ITEMTYPE_SCEPTER },
	{ ' sq9', ITEMTYPE_SCEPTER },
	{ ' sw9', ITEMTYPE_SCEPTER },
	{ ' ps9', ITEMTYPE_MACE },
	{ ' am9', ITEMTYPE_MACE },
	{ ' tm9', ITEMTYPE_MACE },
	{ ' lf9', ITEMTYPE_MACE },
	{ ' hw9', ITEMTYPE_MACE },
	{ ' 9m9', ITEMTYPE_MACE },
	{ ' mg9', ITEMTYPE_MACE },
	{ ' ss9', ITEMTYPE_SWORD },
	{ ' ms9', ITEMTYPE_SWORD },
	{ ' bs9', ITEMTYPE_SWORD },
	{ ' cf9', ITEMTYPE_SWORD },
	{ ' rc9', ITEMTYPE_SWORD },
	{ ' sb9', ITEMTYPE_SWORD },
	{ ' sl9', ITEMTYPE_SWORD },
	{ ' dw9', ITEMTYPE_SWORD },
	{ ' h29', ITEMTYPE_SWORD },
	{ ' mc9', ITEMTYPE_SWORD },
	{ ' sg9', ITEMTYPE_SWORD },
	{ ' 9b9', ITEMTYPE_SWORD },
	{ ' bf9', ITEMTYPE_SWORD },
	{ ' dg9', ITEMTYPE_SWORD },
	{ ' gd9', ITEMTYPE_KNIFE },
	{ ' id9', ITEMTYPE_KNIFE },
	{ ' rk9', ITEMTYPE_KNIFE },
	{ ' lb9', ITEMTYPE_KNIFE },
	{ ' kt9', ITEMTYPE_THROWING_KNIFE },
	{ ' at9', ITEMTYPE_THROWING_AXE },
	{ ' kb9', ITEMTYPE_THROWING_KNIFE },
	{ ' 8b9', ITEMTYPE_THROWING_AXE },
	{ ' aj9', ITEMTYPE_SPEAR },
	{ ' ip9', ITEMTYPE_SPEAR },
	{ ' 9s9', ITEMTYPE_SPEAR },
	{ ' lg9', ITEMTYPE_SPEAR },
	{ ' st9', ITEMTYPE_SPEAR },
	{ ' rs9', ITEMTYPE_SPEAR },
	{ ' rt9', ITEMTYPE_SPEAR },
	{ ' rb9', ITEMTYPE_SPEAR },
	{ ' ts9', ITEMTYPE_SPEAR },
	{ ' 9p9', ITEMTYPE_SPEAR },
	{ ' 7b9', ITEMTYPE_POLEARM },
	{ ' ov9', ITEMTYPE_POLEARM },
	{ ' 8s9', ITEMTYPE_POLEARM },
	{ ' ap9', ITEMTYPE_POLEARM },
	{ ' 9h9', ITEMTYPE_POLEARM },
	{ ' cw9', ITEMTYPE_POLEARM },
	{ ' ss8', ITEMTYPE_STAFF },
	{ ' sl8', ITEMTYPE_STAFF },
	{ ' sc8', ITEMTYPE_STAFF },
	{ ' sb8', ITEMTYPE_STAFF },
	{ ' sw8', ITEMTYPE_STAFF },
	{ ' bs8', ITEMTYPE_BOW },
	{ ' bh8', ITEMTYPE_BOW },
	{ ' bl8', ITEMTYPE_BOW },
	{ ' bc8', ITEMTYPE_BOW },
	{ ' 8s8', ITEMTYPE_BOW },
	{ ' 8l8', ITEMTYPE_BOW },
	{ ' ws8', ITEMTYPE_BOW },
	{ ' wl8', ITEMTYPE_BOW },
	{ ' xl8', ITEMTYPE_CROSSBOW },
	{ ' xm8', ITEMTYPE_CROSSBOW },
	{ ' xh8', ITEMTYPE_CROSSBOW },
	{ ' xr8', ITEMTYPE_CROSSBOW },
	{ ' pax', ITEMTYPE_HELM },
	{ ' pkx', ITEMTYPE_HELM },
	{ ' mlx', ITEMTYPE_HELM },
	{ ' lhx', ITEMTYPE_HELM },
	{ ' mhx', ITEMTYPE_HELM },
	{ ' nrx', ITEMTYPE_HELM },
	{ ' ksx', ITEMTYPE_HELM },
	{ ' iux', ITEMTYPE_ARMOR },
	{ ' aex', ITEMTYPE_ARMOR },
	{ ' alx', ITEMTYPE_ARMOR },
	{ ' utx', ITEMTYPE_ARMOR },
	{ ' gnx', ITEMTYPE_ARMOR },
	{ ' lcx', ITEMTYPE_ARMOR },
	{ ' nhx', ITEMTYPE_ARMOR },
	{ ' srx', ITEMTYPE_ARMOR },
	{ ' lpx', ITEMTYPE_ARMOR },
	{ ' tlx', ITEMTYPE_ARMOR },
	{ ' dlx', ITEMTYPE_ARMOR },
	{ ' htx', ITEMTYPE_ARMOR },
	{ ' lux', ITEMTYPE_ARMOR },
	{ ' rax', ITEMTYPE_ARMOR },
	{ ' ptx', ITEMTYPE_ARMOR },
	{ ' cux', ITEMTYPE_SHIELD },
	{ ' lmx', ITEMTYPE_SHIELD },
	{ ' grx', ITEMTYPE_SHIELD },
	{ ' tix', ITEMTYPE_SHIELD },
	{ ' wox', ITEMTYPE_SHIELD },
	{ ' stx', ITEMTYPE_SHIELD },
	{ ' glx', ITEMTYPE_GLOVES },
	{ ' gvx', ITEMTYPE_GLOVES },
	{ ' gmx', ITEMTYPE_GLOVES },
	{ ' gtx', ITEMTYPE_GLOVES },
	{ ' ghx', ITEMTYPE_GLOVES },
	{ ' blx', ITEMTYPE_BOOTS },
	{ ' bvx', ITEMTYPE_BOOTS },
	{ ' bmx', ITEMTYPE_BOOTS },
	{ ' btx', ITEMTYPE_BOOTS },
	{ ' bhx', ITEMTYPE_BOOTS },
	{ ' blz', ITEMTYPE_BELT },
	{ ' bvz', ITEMTYPE_BELT },
	{ ' bmz', ITEMTYPE_BELT },
	{ ' btz', ITEMTYPE_BELT },
	{ ' bhz', ITEMTYPE_BELT },
	{ ' 9hx', ITEMTYPE_HELM },
	{ ' hsx', ITEMTYPE_SHIELD },
	{ ' kpx', ITEMTYPE_SHIELD },
	{ ' 1fq', ITEMTYPE_MACE },
	{ ' 2fq', ITEMTYPE_MACE },
};

// Sanitizer-flagged: INVENTORY_InitializeComponentArray's
// free-slot search (below, "gTxtComponentItemTypeMap[nIndex].dwCode") increments nIndex with no bound
// check while it runs and only clamps it back to nCurrentTableEntries afterwards, so a game whose
// Items.txt fills every one of this table's slots reads one past the end before the clamp fires.
// Retail's own compiled 1.13c D2Common.dll has the identical shape (Ghidra decompile of VA
// 0x6FD6E920, confirmed against ~/d2-retail/D2Common.dll: the same unguarded "look for an empty slot"
// loop, the same `if (local_18 > 0xFE) local_18 = local_10;` clamp applied only once the loop exits),
// and D2MOO's 1.10f decompile (reference/D2MOO/source/D2Common/src/D2Inventory.cpp) carries the same
// unguarded loop unchanged -- this is retail's own long-standing read, not a defect this port
// introduced. The one slot retail reads past the array (VA 0x6FDF0BA8 + 255*8 = 0x6FDF13A0) is not
// part of this table at all: it is `gnComponentArrayRecordCount`'s own storage, the very next global
// the retail linker placed after this array (matching this file's separately-declared
// gnComponentArrayRecordCount below), and INVENTORY_InitializeComponentArray is its only writer,
// assigning it 255 once, after this same scan loop finishes. So both on the one call that runs the
// scan and on every later read, retail's one-past value is that variable's own pristine
// never-yet-assigned zero. A trailing zero entry reproduces that value in-bounds; the logical size
// every bound check below compares against stays 255, matching retail's own clamp.
static constexpr int kComponentArrayLogicalSize = 255;
D2InventoryComponentItemTypeStrc gTxtComponentItemTypeMap[kComponentArrayLogicalSize + 1] = {};

//D2Common.0x6FDEAF00
int gnComponentArrayRecordCount;
//D2Common.0x6FDEAF04
BOOL gbComponentArrayInitialized;


// 1.13c D2Common+0x21640
//D2Common.0x6FD8E210
BOOL __fastcall INVENTORY_RemoveItem(D2UnitStrc* pItem)
{
	D2ItemExtraDataStrc* pItemExtraData = INVENTORY_GetItemExtraDataFromItem(pItem);

	if (!pItemExtraData || !INVENTORY_GetPtrIfValid(pItemExtraData->pParentInv))
	{
		return FALSE;
	}

	D2InventoryStrc* const pInventory = pItemExtraData->pParentInv;

	if (pItemExtraData->nNodePos > 0)
	{
		const int nNodePos = pItemExtraData->nNodePos - 1;
		if (pInventory->nGridCount > nNodePos)
		{
			D2InventoryGridStrc* pInventoryGrid = &pInventory->pGrids[nNodePos];

			if (pItemExtraData->pPreviousGridItem)
			{
				D2ItemExtraDataStrc* pExtraData = INVENTORY_GetItemExtraDataFromItem(pItemExtraData->pPreviousGridItem);
				if (pExtraData)
				{
					pExtraData->pNextGridItem = pItemExtraData->pNextGridItem;
				}
			}
			else
			{
				if (pInventoryGrid->pItem == pItem)
				{
					pInventoryGrid->pItem = pItemExtraData->pNextGridItem;
				}
			}

			if (pItemExtraData->pNextGridItem)
			{
				D2ItemExtraDataStrc* pExtraData = INVENTORY_GetItemExtraDataFromItem(pItemExtraData->pNextGridItem);
				if (pExtraData)
				{
					pExtraData->pPreviousGridItem = pItemExtraData->pPreviousGridItem;
				}
			}
			else
			{
				if (pInventoryGrid->pLastItem == pItem)
				{
					pInventoryGrid->pLastItem = pItemExtraData->pPreviousGridItem;
				}
			}

			if (pInventoryGrid->ppItems)
			{
				D2CoordStrc pCoords = {};

				UNITS_GetCoords(pItem, &pCoords);

				if (nNodePos < 1)
				{
					pInventoryGrid->ppItems[pCoords.nX + pCoords.nY * pInventoryGrid->nGridWidth] = nullptr;
				}
				else
				{
					uint8_t nHeight = 0;
					uint8_t nWidth = 0;

					ITEMS_GetDimensions(pItem, &nWidth, &nHeight, __FILE__, __LINE__);

					for (int y = pCoords.nY; y < pCoords.nY + nHeight; ++y)
					{
						for (int x = pCoords.nX; x < pCoords.nX + nWidth; ++x)
						{
							pInventoryGrid->ppItems[y * pInventoryGrid->nGridWidth + x] = nullptr;
						}
					}
				}
			}

			pItemExtraData->pNextGridItem = nullptr;
			pItemExtraData->pPreviousGridItem = nullptr;
			pItemExtraData->nNodePos = 0;
		}
	}

	if (pInventory->pCursorItem == pItem)
	{
		pInventory->pCursorItem = nullptr;
	}
	else
	{
		if (pItemExtraData->pPreviousItem)
		{
			D2ItemExtraDataStrc* pExtraData = INVENTORY_GetItemExtraDataFromItem(pItemExtraData->pPreviousItem);
			if (pExtraData)
			{
				pExtraData->pNextItem = pItemExtraData->pNextItem;
			}
		}
		else
		{
			if (pInventory->pFirstItem == pItem)
			{
				pInventory->pFirstItem = pItemExtraData->pNextItem;
			}
		}

		if (pItemExtraData->pNextItem)
		{
			D2ItemExtraDataStrc* pExtraData = INVENTORY_GetItemExtraDataFromItem(pItemExtraData->pNextItem);
			if (pExtraData)
			{
				pExtraData->pPreviousItem = pItemExtraData->pPreviousItem;
			}
		}
		else
		{
			if (pInventory->pLastItem == pItem)
			{
				pInventory->pLastItem = pItemExtraData->pPreviousItem;
			}
		}

		pItemExtraData->pNextItem = nullptr;
		pItemExtraData->pPreviousItem = nullptr;

		--pInventory->dwItemCount;
	}

	// Retail detaches before owner/stat cleanup, which can invoke removal callbacks.
	pItemExtraData->pParentInv = nullptr;
	pItemExtraData->nNodePos = 0;
	pItemExtraData->nNodePosOther = 0;

	ITEMS_SetOwnerId(pItem, -1);

	if (pInventory->pOwner)
	{
		STATLIST_ExpireUnitStatlist(pInventory->pOwner, pItem);
	}

	if (pInventory->dwLeftItemGUID == pItem->dwUnitId)
	{
		pInventory->dwLeftItemGUID = D2UnitInvalidGUID;
	}


	return TRUE;
}

//D2Common.0x6FD8E4A0
// 1.13c D2Common+0x1E790
D2ItemExtraDataStrc* __fastcall INVENTORY_GetItemExtraDataFromItem(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc* pItemData = ITEMS_GetItemData(pItem); INVENTORY_HasItemExtraData(pItemData))
	{
		return &pItemData->pExtraData;
	}
	
	return nullptr;
}

//D2Common.0x6FD8E4C0 (#10240)
// 1.13c D2Common+0x1E710
D2InventoryStrc* __stdcall INVENTORY_AllocInventory(void* pMemPool, D2UnitStrc* pOwner)
{
	if (!pMemPool && pOwner)
	{
		pMemPool = pOwner->pMemoryPool;
	}

	D2InventoryStrc* pInventory = D2_ALLOC_STRC_POOL(pMemPool, D2InventoryStrc);
	if (pInventory)
	{
		memset(pInventory, 0x00, sizeof(D2InventoryStrc));

		pInventory->dwSignature = D2C_InventoryHeader;
		pInventory->pMemPool = pMemPool;
		pInventory->pOwner = pOwner;

		if (pOwner)
		{
			pOwner->pInventory = pInventory;
			pInventory->dwOwnerGuid = pOwner->dwUnitId;
		}
	}

	return pInventory;
}

//D2Common.0x6FD8E520 (#10241)
// 1.13c D2Common+0x21B40
void __stdcall INVENTORY_FreeInventory(D2InventoryStrc* pInventory)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		if (pInventory->pCursorItem)
		{
			INVENTORY_RemoveItem(pInventory->pCursorItem);
		}

		for (D2UnitStrc* pItem = pInventory->pFirstItem; pItem; pItem = pInventory->pFirstItem)
		{
			INVENTORY_RemoveItem(pItem);
		}

		for (int i = 0; i < pInventory->nGridCount; ++i)
		{
			if (pInventory->pGrids[i].ppItems)
			{
				D2_FREE_POOL(pInventory->pMemPool, pInventory->pGrids[i].ppItems);
			}
		}

		if (pInventory->pGrids)
		{
			D2_FREE_POOL(pInventory->pMemPool, pInventory->pGrids);
		}

		if (pInventory->pOwner)
		{
			pInventory->pOwner->pInventory = nullptr;
		}
		
		D2InventoryNodeStrc* pNextNode = nullptr;
		for (D2InventoryNodeStrc* pNode = pInventory->pFirstNode; pNode; pNode = pNextNode)
		{
			pNextNode = pNode->pNext;
			D2_FREE_POOL(pInventory->pMemPool, pNode);
			
		}

		D2CorpseStrc* pNextCorpse = nullptr;
		for (D2CorpseStrc* pCorpse = pInventory->pFirstCorpse; pCorpse; pCorpse = pNextCorpse)
		{
			pNextCorpse = pCorpse->pNextCorpse;
			D2_FREE_POOL(pInventory->pMemPool, pCorpse);
		}

		D2_FREE_POOL(pInventory->pMemPool, pInventory);
	}
}

//D2Common.0x6FD8E620 (#10244)
// 1.13c D2Common+0x1F6E0
BOOL __stdcall INVENTORY_CompareWithItemsParentInventory(D2InventoryStrc* pInventory, D2UnitStrc* pItem)
{
	if (pInventory)
	{
		D2ItemExtraDataStrc* pItemExtraData = INVENTORY_GetItemExtraDataFromItem(pItem);
		// 1.13c: no extra data (no item, not an item, no data) is no match; 1.10f dereferenced it
		return pItemExtraData && pItemExtraData->pParentInv == pInventory;
	}

	return FALSE;
}

//D2Common.0x6FD8E660 (#10243)
// 1.13c D2Common+0x21B00
D2UnitStrc* __stdcall INVENTORY_RemoveItemFromInventory(D2InventoryStrc* pInventory, D2UnitStrc* pItem)
{
	if (INVENTORY_CompareWithItemsParentInventory(pInventory, pItem))
	{
		INVENTORY_RemoveItem(pItem);
		return pItem;
	}
	
	return nullptr;
}

//D2Common.0x6FD8E6A0 (#10242)
// 1.13c D2Common+0x217F0
BOOL __stdcall INVENTORY_PlaceItemInSocket(D2InventoryStrc* pInventory, D2UnitStrc* pItem, int nUnused)
{
	D2_MAYBE_UNUSED(nUnused);
	if (!INVENTORY_GetPtrIfValid(pInventory))
	{
		return FALSE;
	}

	// 1.13c order: no owner -> TRUE; item missing or not an item -> FALSE; owner not an item -> TRUE; no item data -> FALSE
	if (!pInventory->pOwner)
	{
		return TRUE;
	}

	if (!pItem || pItem->dwUnitType != UNIT_ITEM)
	{
		return FALSE;
	}

	if (pInventory->pOwner->dwUnitType != UNIT_ITEM)
	{
		return TRUE;
	}

	D2ItemExtraDataStrc* pItemExtraData = INVENTORY_GetItemExtraDataFromItem(pItem);
	if (!pItemExtraData)
	{
		return FALSE;
	}

	INVENTORY_RemoveItem(pItem);

	D2UnitStrc* pLastItem = pInventory->pLastItem;
	if (pLastItem)
	{
		D2ItemExtraDataStrc* pLastItemExtraData = INVENTORY_GetItemExtraDataFromItem(pLastItem);
		if (pLastItemExtraData)
		{
			pLastItemExtraData->pNextItem = pItem;
		}
	}
	else
	{
		pInventory->pFirstItem = pItem;
	}

	pItemExtraData->pPreviousItem = pInventory->pLastItem;
	pInventory->pLastItem = pItem;
	pItemExtraData->pParentInv = pInventory;

	UNITS_SetXForStaticUnit(pItem, pInventory->dwItemCount);
	UNITS_SetYForStaticUnit(pItem, 0);

	pItemExtraData->nNodePos = 0;
	pItemExtraData->nNodePosOther = 1;

	++pInventory->dwItemCount;

	ITEMS_SetOwnerId(pItem, -1);

	return TRUE;
}

//D2Common.0x6FD8E7A0 (#10277)
// 1.13c D2Common+0x1E190
D2UnitStrc* __stdcall INVENTORY_GetFirstItem(D2InventoryStrc* pInventory)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		return pInventory->pFirstItem;
	}

	return nullptr;
}

//D2Common.0x6FD8E7C0 (#10278)
// 1.13c D2Common+0x1E170: signature check, then the dword at +0x10; no delta
D2UnitStrc* __stdcall INVENTORY_GetLastItem(D2InventoryStrc* pInventory)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		return pInventory->pLastItem;
	}

	return nullptr;
}

//D2Common.0x6FD8E7E0 (#10245)
// 1.13c D2Common+0x208E0
BOOL __stdcall INVENTORY_GetFreePosition(D2InventoryStrc* pInventory, D2UnitStrc* pItem, int nInventoryRecordId, int* pFreeX, int* pFreeY, uint8_t nPage)
{
	if (!INVENTORY_GetPtrIfValid(pInventory) || !ITEMS_GetItemData(pItem))
	{
		return FALSE;
	}
	
	uint8_t nWidth = 0;
	uint8_t nHeight = 0;
	ITEMS_GetDimensions(pItem, &nWidth, &nHeight, __FILE__, __LINE__);
	if (!nWidth || !nHeight)
	{
		return FALSE;
	}

	D2InventoryGridInfoStrc pInventoryGridInfo = {};
	DATATBLS_GetInventoryGridInfo(nInventoryRecordId, 0, &pInventoryGridInfo);

	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, nPage + INVGRID_INVENTORY, &pInventoryGridInfo);
	if (!pInventoryGrid)
	{
		return FALSE;
	}

	// 1.13c picks a per-shape wrapper (the owner test lives in each).
	if (nHeight == 1)
	{
		return INVENTORY_FindFreePositionForOwnerFlat(pInventory, pInventoryGrid, pFreeX, pFreeY, nWidth, 1);
	}
	if (nHeight == 3 && nWidth < 3)
	{
		return INVENTORY_FindFreePositionForOwnerTall(pInventory, pInventoryGrid, pFreeX, pFreeY, nWidth, 3);
	}
	if (nHeight == 2 && nWidth == 2)
	{
		return INVENTORY_FindFreePositionForOwnerSquare(pInventory, pInventoryGrid, pFreeX, pFreeY, 2, 2);
	}

	return INVENTORY_FindFreePositionForOwner(pInventory, pInventoryGrid, pFreeX, pFreeY, nWidth, nHeight);
}

//D2Common.0x6FD8EAF0
// 1.13c D2Common+0x1E1B0
D2InventoryGridStrc* __fastcall INVENTORY_GetGrid(D2InventoryStrc* pInventory, int nInventoryGrid, D2InventoryGridInfoStrc* pInventoryGridInfo)
{
	D2InventoryGridStrc* pInventoryGrid = nullptr;

	if (pInventory->nGridCount <= nInventoryGrid)
	{
		if (!pInventoryGridInfo)
		{
			return nullptr;
		}

		pInventory->pGrids = (D2InventoryGridStrc*)D2_REALLOC_POOL(pInventory->pMemPool, pInventory->pGrids, sizeof(D2InventoryGridStrc) * (nInventoryGrid + 1));
		memset(&pInventory->pGrids[pInventory->nGridCount], 0x00, sizeof(D2InventoryGridStrc) * (nInventoryGrid - pInventory->nGridCount + 1));
		pInventory->nGridCount = nInventoryGrid + 1;

		pInventoryGrid = &pInventory->pGrids[nInventoryGrid];
		pInventoryGrid->nGridWidth = pInventoryGridInfo->nGridX;
		pInventoryGrid->nGridHeight = pInventoryGridInfo->nGridY;
		pInventoryGrid->ppItems = (D2UnitStrc**)D2_ALLOC_POOL(pInventory->pMemPool, sizeof(D2UnitStrc*) * pInventoryGrid->nGridHeight * pInventoryGrid->nGridWidth);
		memset(pInventoryGrid->ppItems, 0x00, sizeof(D2UnitStrc*) * pInventoryGrid->nGridHeight * pInventoryGrid->nGridWidth);
	}
	else
	{
		pInventoryGrid = &pInventory->pGrids[nInventoryGrid];
		if (pInventoryGridInfo)
		{
			if (!pInventoryGrid->ppItems)
			{
				pInventoryGrid->nGridWidth = pInventoryGridInfo->nGridX;
				pInventoryGrid->nGridHeight = pInventoryGridInfo->nGridY;
				pInventoryGrid->ppItems = (D2UnitStrc**)D2_ALLOC_POOL(pInventory->pMemPool, sizeof(D2UnitStrc*) * pInventoryGrid->nGridHeight * pInventoryGrid->nGridWidth);
				memset(pInventoryGrid->ppItems, 0x00, sizeof(D2UnitStrc*) * pInventoryGrid->nGridHeight * pInventoryGrid->nGridWidth);
			}

			if (pInventoryGrid->nGridWidth != pInventoryGridInfo->nGridX || pInventoryGrid->nGridHeight != pInventoryGridInfo->nGridY)
			{
				return nullptr;
			}
		}
		else if (!pInventoryGrid->ppItems)
		{
			return nullptr;
		}
	}

	return pInventoryGrid;
}

//D2Common.0x6FD8EC70
// not-1.13c: retail folds the bounds test and the anchor-cell test into its placement search (D2Common+0x1F580), then calls the cell test (0x1E360)
BOOL __fastcall INVENTORY_CanItemBePlacedAtPos(D2InventoryGridStrc* pInventoryGrid, int nX, int nY, uint8_t nItemWidth, uint8_t nItemHeight)
{
	if (nItemWidth + nX > pInventoryGrid->nGridWidth || nItemHeight + nY > pInventoryGrid->nGridHeight)
	{
		return FALSE;
	}

	return INVENTORY_AreCellsFree(pInventoryGrid, nX, nY, nItemWidth, nItemHeight);
}

// The unchecked cell test inside the placement test: every cell of the item's footprint is empty. No bounds test
// (the caller makes it); cells are read by row stride, so a footprint past the edge reads the cells after it.
// 1.13c D2Common+0x1E360
BOOL __fastcall INVENTORY_AreCellsFree(D2InventoryGridStrc* pInventoryGrid, int nX, int nY, uint8_t nItemWidth, uint8_t nItemHeight)
{
	for (int nTestY = 0; nTestY < nItemHeight; ++nTestY)
	{
		for (int nTestX = 0; nTestX < nItemWidth; ++nTestX)
		{
			if (pInventoryGrid->ppItems[(nX + nTestX) + (nY + nTestY) * pInventoryGrid->nGridWidth])
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

// 1.13c: the four grid finders test the anchor cell before the bounds and footprint tests, so an item with a zero
// width or height is still refused on an occupied anchor (1.10f's placement test alone accepts it there).
static BOOL INVENTORY_FinderFits(D2InventoryGridStrc* pInventoryGrid, int nX, int nY, uint8_t nItemWidth, uint8_t nItemHeight)
{
	return !pInventoryGrid->ppItems[nX + nY * pInventoryGrid->nGridWidth] && INVENTORY_CanItemBePlacedAtPos(pInventoryGrid, nX, nY, nItemWidth, nItemHeight);
}

//D2Common.0x6FD8ECF0
// 1.13c D2Common+0x1F580
BOOL __fastcall INVENTORY_FindFreePositionBottomRightToTopLeftWithWeight(D2InventoryGridStrc* pInventoryGrid, int* pFreeX, int* pFreeY, uint8_t nItemWidth, uint8_t nItemHeight)
{
	if (pInventoryGrid->nGridWidth < 1 || pInventoryGrid->nGridHeight < 1)
	{
		return FALSE;
	}

	uint8_t nMax = 0;
	for (int nX = pInventoryGrid->nGridWidth - 1; nX >= 0; --nX)
	{
		for (int nY = pInventoryGrid->nGridHeight - 1; nY >= 0; --nY)
		{
			if (INVENTORY_FinderFits(pInventoryGrid, nX, nY, nItemWidth, nItemHeight))
			{
				const uint8_t nWeight = INVENTORY_GetPlacementWeight(pInventoryGrid, nX, nY, nItemWidth, nItemHeight);
				if (nWeight > nMax)
				{
					nMax = nWeight;

					*pFreeX = nX;
					*pFreeY = nY;

					if (nWeight == 255)
					{
						return TRUE;
					}
				}
			}
		}
	}

	return nMax > 0;
}

//D2Common.0x6FD8EE20
// 1.13c D2Common+0x1E010: validated, no delta (four-side neighbour count, off-grid side counts its full length, 255 when fully enclosed)
uint8_t __fastcall INVENTORY_GetPlacementWeight(D2InventoryGridStrc* pInventoryGrid, int nXPos, int nYPos, uint8_t nItemWidth, uint8_t nItemHeight)
{
	uint8_t nResult = 0;

	if (nXPos > 0)
	{
		for (int i = 0; i < nItemHeight; ++i)
		{
			if (pInventoryGrid->ppItems[nXPos - 1 + (nYPos + i) * pInventoryGrid->nGridWidth])
			{
				++nResult;
			}
		}
	}
	else
	{
		nResult = nItemHeight;
	}

	if (nXPos + nItemWidth < pInventoryGrid->nGridWidth)
	{
		for (int i = 0; i < nItemHeight; ++i)
		{
			if (pInventoryGrid->ppItems[nXPos + nItemWidth + (nYPos + i) * pInventoryGrid->nGridWidth])
			{
				++nResult;
			}
		}
	}
	else
	{
		nResult += nItemHeight;
	}

	if (nYPos > 0)
	{
		for (int i = 0; i < nItemWidth; ++i)
		{
			if (pInventoryGrid->ppItems[(nXPos + i) + (nYPos - 1) * pInventoryGrid->nGridWidth])
			{
				++nResult;
			}
		}
	}
	else
	{
		nResult += nItemWidth;
	}

	if (nYPos + nItemHeight < pInventoryGrid->nGridHeight)
	{
		for (int i = 0; i < nItemWidth; ++i)
		{
			if (pInventoryGrid->ppItems[(nXPos + i) + (nYPos + nItemHeight) * pInventoryGrid->nGridWidth])
			{
				++nResult;
			}
		}
	}
	else
	{
		nResult += nItemWidth;
	}

	if (nResult >= 2 * (nItemHeight + nItemWidth))
	{
		nResult = 255;
	}

	return nResult;
}

//D2Common.0x6FD8EFB0
// 1.13c D2Common+0x1F410
BOOL __fastcall INVENTORY_FindFreePositionTopLeftToBottomRightWithWeight(D2InventoryGridStrc* pInventoryGrid, int* pFreeX, int* pFreeY, uint8_t nItemWidth, uint8_t nItemHeight)
{
	if (pInventoryGrid->nGridWidth <= 0 || pInventoryGrid->nGridHeight <= 0)
	{
		return FALSE;
	}

	uint8_t nMax = 0;
	for (int nY = 0; nY < pInventoryGrid->nGridHeight; ++nY)
	{
		for (int nX = 0; nX < pInventoryGrid->nGridWidth; ++nX)
		{
			if (INVENTORY_FinderFits(pInventoryGrid, nX, nY, nItemWidth, nItemHeight))
			{
				const uint8_t nWeight = INVENTORY_GetPlacementWeight(pInventoryGrid, nX, nY, nItemWidth, nItemHeight);
				if (nWeight > nMax)
				{
					nMax = nWeight;

					*pFreeX = nX;
					*pFreeY = nY;

					if (nWeight == 255)
					{
						return TRUE;
					}
				}
			}
		}
	}

	return nMax > 0;
}

//D2Common.0x6FD8F0E0
// 1.13c D2Common+0x1F370
BOOL __fastcall INVENTORY_FindFreePositionTopLeftToBottomRight(D2InventoryGridStrc* pInventoryGrid, int* pFreeX, int* pFreeY, uint8_t nItemWidth, uint8_t nItemHeight)
{
	for (int nX = 0; nX < pInventoryGrid->nGridWidth; ++nX)
	{
		for (int nY = 0; nY < pInventoryGrid->nGridHeight; ++nY)
		{
			if (INVENTORY_FinderFits(pInventoryGrid, nX, nY, nItemWidth, nItemHeight))
			{
				*pFreeX = nX;
				*pFreeY = nY;
				return TRUE;
			}
		}
	}

	return FALSE;
}

// 1.13c has this first-fit scan (columns right to left, each column top to bottom) as its own function; 1.10f inlines it
// in INVENTORY_GetFreePosition.
// 1.13c D2Common+0x1F4E0
BOOL __fastcall INVENTORY_FindFreePositionTopRightToBottomLeft(D2InventoryGridStrc* pInventoryGrid, int* pFreeX, int* pFreeY, uint8_t nItemWidth, uint8_t nItemHeight)
{
	for (int nX = pInventoryGrid->nGridWidth - 1; nX >= 0; --nX)
	{
		for (int nY = 0; nY < pInventoryGrid->nGridHeight; ++nY)
		{
			if (INVENTORY_FinderFits(pInventoryGrid, nX, nY, nItemWidth, nItemHeight))
			{
				*pFreeX = nX;
				*pFreeY = nY;
				return TRUE;
			}
		}
	}

	return FALSE;
}

// 1.13c splits 1.10f's owner test out of INVENTORY_GetFreePosition into four per-shape wrappers: a player's inventory
// takes the weighted scan, any other owner (or none) the first fit. A null inventory finds nothing. Three of the four
// share one body (top-left scans); the one-row shape scans from the right.
static BOOL INVENTORY_OwnerIsPlayer(D2InventoryStrc* pInventory)
{
	return pInventory->pOwner && pInventory->pOwner->dwUnitType == UNIT_PLAYER;
}

// 1.13c D2Common+0x20520
BOOL __fastcall INVENTORY_FindFreePositionForOwner(D2InventoryStrc* pInventory, D2InventoryGridStrc* pInventoryGrid, int* pFreeX, int* pFreeY, uint8_t nItemWidth, uint8_t nItemHeight)
{
	if (!pInventory)
	{
		return FALSE;
	}

	return INVENTORY_OwnerIsPlayer(pInventory)
		? INVENTORY_FindFreePositionTopLeftToBottomRightWithWeight(pInventoryGrid, pFreeX, pFreeY, nItemWidth, nItemHeight)
		: INVENTORY_FindFreePositionTopLeftToBottomRight(pInventoryGrid, pFreeX, pFreeY, nItemWidth, nItemHeight);
}

// 1.13c D2Common+0x20560
BOOL __fastcall INVENTORY_FindFreePositionForOwnerSquare(D2InventoryStrc* pInventory, D2InventoryGridStrc* pInventoryGrid, int* pFreeX, int* pFreeY, uint8_t nItemWidth, uint8_t nItemHeight)
{
	return INVENTORY_FindFreePositionForOwner(pInventory, pInventoryGrid, pFreeX, pFreeY, nItemWidth, nItemHeight);
}

// 1.13c D2Common+0x205A0
BOOL __fastcall INVENTORY_FindFreePositionForOwnerTall(D2InventoryStrc* pInventory, D2InventoryGridStrc* pInventoryGrid, int* pFreeX, int* pFreeY, uint8_t nItemWidth, uint8_t nItemHeight)
{
	return INVENTORY_FindFreePositionForOwner(pInventory, pInventoryGrid, pFreeX, pFreeY, nItemWidth, nItemHeight);
}

// 1.13c D2Common+0x205E0
BOOL __fastcall INVENTORY_FindFreePositionForOwnerFlat(D2InventoryStrc* pInventory, D2InventoryGridStrc* pInventoryGrid, int* pFreeX, int* pFreeY, uint8_t nItemWidth, uint8_t nItemHeight)
{
	if (!pInventory)
	{
		return FALSE;
	}

	return INVENTORY_OwnerIsPlayer(pInventory)
		? INVENTORY_FindFreePositionBottomRightToTopLeftWithWeight(pInventoryGrid, pFreeX, pFreeY, nItemWidth, nItemHeight)
		: INVENTORY_FindFreePositionTopRightToBottomLeft(pInventoryGrid, pFreeX, pFreeY, nItemWidth, nItemHeight);
}

// function probe hooks (src/core/fn/d2common_21e10.cpp): the capture answered 0x208E0 and 0x218A0 with recorders; null in the game.
BOOL (__stdcall* g_pfnPlaceAtFreeGetFreePosition)(D2InventoryStrc*, D2UnitStrc*, int, int*, int*, uint32_t) = NULL;
BOOL (__fastcall* g_pfnPlaceAtFreePlaceInGrid)(D2InventoryStrc*, D2UnitStrc*, int, int, int, int) = NULL;

//D2Common.0x6FD8F1E0 (#10246)
// 1.13c D2Common+0x21E10
// 1.13c: the page word reaches 0x208E0 whole; only the grid index (low byte + 2) is narrowed. bUnused reaches neither callee.
BOOL __stdcall INVENTORY_PlaceItemAtFreePosition(D2InventoryStrc* pInventory, D2UnitStrc* pItem, int nInventoryRecordId, BOOL bUnused, uint32_t nPage, const char* szFile, int nLine)
{
	D2_MAYBE_UNUSED(bUnused);
	int nX = 0;
	int nY = 0;
	if (!pInventory || !INVENTORY_UnitIsItem(pItem))
	{
		return FALSE;
	}

	const BOOL bFree = g_pfnPlaceAtFreeGetFreePosition
		? g_pfnPlaceAtFreeGetFreePosition(pInventory, pItem, nInventoryRecordId, &nX, &nY, nPage)
		: INVENTORY_GetFreePosition(pInventory, pItem, nInventoryRecordId, &nX, &nY, (uint8_t)nPage);
	if (!bFree)
	{
		return FALSE;
	}

	const int nGrid = (int)(nPage & 0xFF) + 2;
	return g_pfnPlaceAtFreePlaceInGrid
		? g_pfnPlaceAtFreePlaceInGrid(pInventory, pItem, nX, nY, nGrid, nInventoryRecordId)
		: INVENTORY_PlaceItemInGrid(pInventory, pItem, nX, nY, nGrid, nInventoryRecordId, 0);
}

//D2Common.0x6FD8F250
// 1.13c D2Common+0x218a0
BOOL __fastcall INVENTORY_PlaceItemInGrid(D2InventoryStrc* pInventory, D2UnitStrc* pItem, int nXPos, int nYPos, int nInventoryGrid, int nInventoryRecordId, BOOL bUnused)
{
	D2_MAYBE_UNUSED(bUnused);
	if (!INVENTORY_GetPtrIfValid(pInventory))
	{
		return FALSE;
	}

	D2ItemExtraDataStrc* pItemExtraData = INVENTORY_GetItemExtraDataFromItem(pItem);
	if (!pItemExtraData)
	{
		return FALSE;
	}

	uint8_t nHeight = 0;
	uint8_t nWidth = 0;
	ITEMS_GetDimensions(pItem, &nWidth, &nHeight, __FILE__, __LINE__);
	if (!nWidth || !nHeight)
	{
		return FALSE;
	}

	D2InventoryGridStrc* pInventoryGrid = nullptr;
	if (nInventoryGrid < INVGRID_INVENTORY)
	{
		nWidth = 1;
		nHeight = 1;
		if (nInventoryGrid == INVGRID_BODYLOC)
		{
			pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
		}
		else if (nInventoryGrid == INVGRID_BELT)
		{
			pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BELT, &gBeltInventoryGridInfo);
		}
	}
	else
	{
		D2InventoryGridInfoStrc pInventoryGridInfo = {};
		DATATBLS_GetInventoryGridInfo(nInventoryRecordId, 0, &pInventoryGridInfo);
		if (nXPos < 0 || nXPos + nWidth > pInventoryGridInfo.nGridX || nYPos < 0 || nYPos + nHeight > pInventoryGridInfo.nGridY)
		{
			return FALSE;
		}

		pInventoryGrid = INVENTORY_GetGrid(pInventory, nInventoryGrid, &pInventoryGridInfo);
	}

	if (!pInventoryGrid)
	{
		return FALSE;
	}

	if (!INVENTORY_CanItemBePlacedAtPos(pInventoryGrid, nXPos, nYPos, nWidth, nHeight))
	{
		return FALSE;
	}

	if (pItem->dwAnimMode == IMODE_ONGROUND && UNITS_GetRoom(pItem))
	{
		if (pItem->dwFlagEx & UNITFLAGEX_SERVERUNIT)
		{
			DUNGEON_AllocDrlgDelete(UNITS_GetRoom(pItem), pItem->dwUnitType, pItem->dwUnitId);
		}

		UNITS_FreeCollisionPath(pItem);
		UNITROOM_RemoveUnitFromRoom(pItem);
	}

	INVENTORY_RemoveItem(pItem);

	if (pInventory->pLastItem)
	{
		D2ItemExtraDataStrc* pLastItemExtraData = INVENTORY_GetItemExtraDataFromItem(pInventory->pLastItem);
		if (pLastItemExtraData)
		{
			pLastItemExtraData->pNextItem = pItem;
		}
	}
	else
	{
		pInventory->pFirstItem = pItem;
	}

	pItemExtraData->pPreviousItem = pInventory->pLastItem;
	pInventory->pLastItem = pItem;
	pItemExtraData->pParentInv = pInventory;

	if (pInventoryGrid->pLastItem)
	{
		D2ItemExtraDataStrc* pLastItemExtraData = INVENTORY_GetItemExtraDataFromItem(pInventoryGrid->pLastItem);
		if (pLastItemExtraData)
		{
			pLastItemExtraData->pNextGridItem = pItem;
		}
	}
	else
	{
		pInventoryGrid->pItem = pItem;
	}

	pItemExtraData->pPreviousGridItem = pInventoryGrid->pLastItem;
	pInventoryGrid->pLastItem = pItem;

	for (int y = 0; y < nHeight; ++y)
	{
		for (int x = 0; x < nWidth; ++x)
		{
			pInventoryGrid->ppItems[nXPos + x + (nYPos + y) * pInventoryGrid->nGridWidth] = pItem;
		}
	}

	pItemExtraData->nNodePos = nInventoryGrid + 1;
	++pInventory->dwItemCount;

	UNITS_SetXForStaticUnit(pItem, nXPos);
	UNITS_SetYForStaticUnit(pItem, nYPos);

	if (pInventory->pOwner && pInventory->pOwner->dwUnitType == UNIT_PLAYER)
	{
		ITEMS_SetOwnerId(pItem, pInventory->pOwner->dwUnitId);
	}
	else
	{
		ITEMS_SetOwnerId(pItem, -1);
	}
	
	if (nInventoryGrid >= 2)
	{
		ITEMS_SetInvPage(pItem, nInventoryGrid - 2);
	}

	if (nInventoryGrid == INVGRID_BODYLOC)
	{
		if (nXPos >= 11)
		{
			pItemExtraData->nNodePosOther = 4;
		}
		else
		{
			pItemExtraData->nNodePosOther = 3;
		}
	}
	else if (nInventoryGrid == INVGRID_BELT)
	{
		pItemExtraData->nNodePosOther = 2;
	}
	else
	{
		pItemExtraData->nNodePosOther = 1;
	}

	return TRUE;
}

//D2Common.0x6FD8F600 (#10247)
// 1.13c D2Common+0x1F1F0
BOOL __stdcall INVENTORY_CanItemBePlaced(D2InventoryStrc* pInventory, D2UnitStrc* pItem, int nXPos, int nYPos, int nInventoryRecordId, D2UnitStrc** ppExchangeItem, unsigned int* pHoveredItems, uint8_t nPage)
{
	*pHoveredItems = 0;

	if (!INVENTORY_GetPtrIfValid(pInventory) || !INVENTORY_UnitIsItem(pItem) || nPage == uint8_t(-1) || nXPos < 0 || nYPos < 0)
	{
		return FALSE;
	}

	uint8_t nHeight = 0;
	uint8_t nWidth = 0;
	ITEMS_GetDimensions(pItem, &nWidth, &nHeight, __FILE__, __LINE__);
	if (!nWidth || !nHeight)
	{
		return FALSE;
	}

	D2InventoryGridInfoStrc pInventoryGridInfo = {};
	DATATBLS_GetInventoryGridInfo(nInventoryRecordId, FALSE, &pInventoryGridInfo);

	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, nPage + 2, &pInventoryGridInfo);
	if (!pInventoryGrid)
	{
		return FALSE;
	}

	BOOL bFree = TRUE;

	for (int nY = nYPos; nY < nYPos + nHeight; ++nY)
	{
		if (nY >= pInventoryGrid->nGridHeight)
		{
			return FALSE;
		}

		for (int nX = nXPos; nX < nXPos + nWidth; ++nX)
		{
			if (nX >= pInventoryGrid->nGridWidth)
			{
				// 1.13c: running off the right edge ends only this row; the next rows are still scanned (hover count and exchange item keep updating)
				bFree = FALSE;
				break;
			}

			D2UnitStrc* pCurrentItem = pInventoryGrid->ppItems[nX + nY * pInventoryGrid->nGridWidth];
			if (pCurrentItem)
			{
				bFree = FALSE;

				if (*ppExchangeItem != pCurrentItem)
				{
					*ppExchangeItem = pCurrentItem;

					++*pHoveredItems;
				}
				else
				{
					break;
				}
			}
		}
	}

	return bFree;
}

//D2Common.0x6FD8F780 (#10248)
// 1.13c D2Common+0x1f040
BOOL __stdcall INVENTORY_CanItemsBeExchanged(D2InventoryStrc* pInventory, D2UnitStrc* pItem, int nXPos, int nYPos, int nInventoryRecordId, D2UnitStrc** ppExchangeItem, uint8_t nPage, BOOL bCheckIfCube)
{
	if (!INVENTORY_GetPtrIfValid(pInventory) || !INVENTORY_UnitIsItem(pItem) || nPage == uint8_t(-1) || nXPos < 0 || nYPos < 0)
	{
		return FALSE;
	}

	uint8_t nHeight = 0;
	uint8_t nWidth = 0;
	ITEMS_GetDimensions(pItem, &nWidth, &nHeight, __FILE__, __LINE__);
	if (!nWidth || !nHeight)
	{
		return FALSE;
	}

	D2InventoryGridInfoStrc pInventoryGridInfo = {};
	DATATBLS_GetInventoryGridInfo(nInventoryRecordId, FALSE, &pInventoryGridInfo);
	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, nPage + 2, &pInventoryGridInfo);
	if (!pInventoryGrid)
	{
		return FALSE;
	}

	const int nXMax = nXPos + nWidth;
	const int nYMax = nYPos + nHeight;

	if (nXMax > pInventoryGrid->nGridWidth || nYMax > pInventoryGrid->nGridHeight)
	{
		return FALSE;
	}

	D2UnitStrc* pLastItem = nullptr;
	int nCounter = 0;
	for (int nY = nYPos; nY < nYMax; ++nY)
	{
		for (int nX = nXPos; nX < nXMax; ++nX)
		{
			D2UnitStrc* pCurrentItem = pInventoryGrid->ppItems[nX + nY * pInventoryGrid->nGridWidth];
			if (pCurrentItem)
			{
				*ppExchangeItem = pCurrentItem;

				if (bCheckIfCube && DATATBLS_GetItemsTxtRecord(pCurrentItem->dwClassId)->dwCode == ' xob')
				{
					return FALSE;
				}

				if (pCurrentItem != pLastItem)
				{
					pLastItem = pCurrentItem;

					++nCounter;
				}
			}
		}
	}

	if (nCounter > 1)
	{
		*ppExchangeItem = nullptr;
		return FALSE;
	}

	return TRUE;
}

//D2Common.0x6FD8F930 (#10249)
// 1.13c D2Common+0x21DD0
BOOL __stdcall INVENTORY_PlaceItemAtInventoryPage(D2InventoryStrc* pInventory, D2UnitStrc* pItem, int nXPos, int nYPos, int nInventoryRecordId, BOOL bUnused, uint8_t nPage)
{
	if (pInventory)
	{
		return INVENTORY_PlaceItemInGrid(pInventory, pItem, nXPos, nYPos, nPage + 2, nInventoryRecordId, bUnused);
	}

	return FALSE;
}

//D2Common.0x6FD8F970 (#10250)
// 1.13c D2Common+0x1DFF0: empty body, eight argument words, ret 0x20
void __stdcall INVENTORY_Return(const char* szFile, int nLine, D2InventoryStrc* pInventory, int nX, int nY, int nInventoryRecordId, BOOL bClient, uint8_t nPage)
{
	return;
}

//D2Common.0x6FD8F980 (#10252)
// 1.13c D2Common+0x1EF90: validated; retail reads the cell position through the shared unit x/y getters (0x1050, 0x1080), same result as the switch below
D2UnitStrc* __stdcall INVENTORY_GetItemFromInventoryPage(D2InventoryStrc* pInventory, int nGridX, int nGridY, int* pX, int* pY, int nInventoryRecordId, uint8_t nPage)
{
	if (!INVENTORY_GetPtrIfValid(pInventory))
	{
		return nullptr;
	}

	D2InventoryGridInfoStrc pInventoryGridInfo = {};
	DATATBLS_GetInventoryGridInfo(nInventoryRecordId, 0, &pInventoryGridInfo);

	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, nPage + 2, &pInventoryGridInfo);
	if (pInventoryGrid && nGridX >= 0 && nGridX <= pInventoryGrid->nGridWidth && nGridY >= 0 && nGridY <= pInventoryGrid->nGridHeight)
	{
		D2UnitStrc* pItem = pInventoryGrid->ppItems[nGridX + nGridY * pInventoryGrid->nGridWidth];
		if (pItem)
		{
			switch (pItem->dwUnitType)
			{
			case UNIT_PLAYER:
			case UNIT_MONSTER:
			case UNIT_MISSILE:
				*pX = pItem->pDynamicPath ? pItem->pDynamicPath->tGameCoords.wPosX : 0;
				*pY = pItem->pDynamicPath ? pItem->pDynamicPath->tGameCoords.wPosY : 0;
				break;

			case UNIT_OBJECT:
			case UNIT_ITEM:
			case UNIT_TILE:
				*pX = pItem->pStaticPath ? pItem->pStaticPath->tGameCoords.nX : 0;
				*pY = pItem->pStaticPath ? pItem->pStaticPath->tGameCoords.nY : 0;
				break;

			default:
				*pX = 0;
				*pY = 0;
				break;
			}

			return pItem;
		}
	}

	return nullptr;
}

//D2Common.0x6FD8FAB0 (#10253)
// 1.13c D2Common+0x21DA0
BOOL __stdcall INVENTORY_PlaceItemInBodyLoc(D2InventoryStrc* pInventory, D2UnitStrc* pItem, int nBodyLoc)
{
	if (pInventory && INVENTORY_ValidateBodyLoc(nBodyLoc))
	{
		return INVENTORY_PlaceItemInGrid(pInventory, pItem, nBodyLoc, 0, INVGRID_BODYLOC, 0, 0);
	}

	return FALSE;
}

//D2Common.0x6FD8FAE0 (#10257)
// 1.13c D2Common+0x1E670: signature check, slot 0..12 of the body grid; no delta
D2UnitStrc* __stdcall INVENTORY_GetItemFromBodyLoc(D2InventoryStrc* pInventory, int nBodyLoc)
{
	if (INVENTORY_GetPtrIfValid(pInventory) && INVENTORY_ValidateBodyLoc(nBodyLoc))
	{
		D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
		if (pInventoryGrid)
		{
			return pInventoryGrid->ppItems[nBodyLoc];
		}
	}

	return nullptr;
}

//D2Common.0x6FD8FB20 (#10255)
// 1.13c D2Common+0x20490
void __stdcall INVENTORY_GetSecondWieldingWeapon(D2UnitStrc* pPlayer, D2InventoryStrc* pInventory, D2UnitStrc** ppItem, int nBodyLoc)
{
	if (INVENTORY_GetPtrIfValid(pInventory) && !*ppItem && INVENTORY_ValidateBodyLoc(nBodyLoc))
	{
		D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
		if (pInventoryGrid)
		{
			if (nBodyLoc == BODYLOC_RARM)
			{
				if (INVENTORY_GetWieldType(pPlayer, pInventory) == 2)
				{
					*ppItem = pInventoryGrid->ppItems[BODYLOC_LARM];
				}
			}
			else if (nBodyLoc == BODYLOC_LARM)
			{
				if (INVENTORY_GetWieldType(pPlayer, pInventory) == 2)
				{
					*ppItem = pInventoryGrid->ppItems[BODYLOC_RARM];
				}
			}
		}
	}
}

//D2Common.0x6FD8FBB0 (#10256)
// 1.13c D2Common+0x203B0: slot 5 then slot 4, equippable and weapon-class match; no delta
BOOL __stdcall INVENTORY_CheckEquipmentForWeaponByClass(D2InventoryStrc* pInventory, int nWeaponClass)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);

		D2UnitStrc* pItem = pInventoryGrid ? pInventoryGrid->ppItems[BODYLOC_LARM] : nullptr;

		if (ITEMS_CanBeEquipped(pItem) && ITEMS_CheckWeaponClass(pItem, nWeaponClass))
		{
			return TRUE;
		}

		pItem = pInventoryGrid ? pInventoryGrid->ppItems[BODYLOC_RARM] : nullptr;

		if (ITEMS_CanBeEquipped(pItem) && ITEMS_CheckWeaponClass(pItem, nWeaponClass))
		{
			return TRUE;
		}
	}

	return FALSE;
}

//D2Common.0x6FD8FC60 (#10258)
// 1.13c D2Common+0x20320: validated, no delta (slot 5 then slot 4, weapon type check, left-hand GUID match)
D2UnitStrc* __stdcall INVENTORY_GetLeftHandWeapon(D2InventoryStrc* pInventory)
{
	if (INVENTORY_GetPtrIfValid(pInventory) && pInventory->dwLeftItemGUID != D2UnitInvalidGUID)
	{
		D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
		if (pInventoryGrid)
		{
			D2UnitStrc* pItem = pInventoryGrid->ppItems[BODYLOC_LARM];
			if (pItem && ITEMS_CheckItemTypeId(pItem, ITEMTYPE_WEAPON) && pItem->dwUnitId == pInventory->dwLeftItemGUID)
			{
				return pItem;
			}

			pItem = pInventoryGrid->ppItems[BODYLOC_RARM];
			if (pItem && ITEMS_CheckItemTypeId(pItem, ITEMTYPE_WEAPON) && pItem->dwUnitId == pInventory->dwLeftItemGUID)
			{
				return pItem;
			}
		}
	}

	return nullptr;
}

//D2Common.0x6FD8FD10 (#11301)
// 1.13c D2Common+0x20270: validated, no delta (weapon in slot 5 and slot 4, return the one that is not the left-hand GUID)
D2UnitStrc* __stdcall INVENTORY_GetSecondaryWeapon(D2InventoryStrc* pInventory)
{
	if (INVENTORY_GetPtrIfValid(pInventory) && pInventory->dwLeftItemGUID != D2UnitInvalidGUID)
	{
		D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
		if (pInventoryGrid)
		{
			D2UnitStrc* pLeftHandItem = pInventoryGrid->ppItems[BODYLOC_LARM];
			if (!pLeftHandItem || !ITEMS_CheckItemTypeId(pLeftHandItem, ITEMTYPE_WEAPON))
			{
				pLeftHandItem = nullptr;
			}

			D2UnitStrc* pRightHandItem = pInventoryGrid->ppItems[BODYLOC_RARM];
			if (!pRightHandItem || !ITEMS_CheckItemTypeId(pRightHandItem, ITEMTYPE_WEAPON))
			{
				pRightHandItem = nullptr;
			}

			if (pLeftHandItem && pLeftHandItem->dwUnitId == pInventory->dwLeftItemGUID)
			{
				return pRightHandItem;
			}

			if (pRightHandItem && pRightHandItem->dwUnitId == pInventory->dwLeftItemGUID)
			{
				return pLeftHandItem;
			}
		}
	}

	return nullptr;
}

//D2Common.0x6FD8FDD0 (#10259)
// 1.13c D2Common+0x20830
D2UnitStrc* __stdcall INVENTORY_GetCompositItem(D2InventoryStrc* pInventory, int nComponent)
{
	if (!INVENTORY_GetPtrIfValid(pInventory))
	{
		return nullptr;
	}

	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
	if (pInventoryGrid)
	{
		D2UnitStrc* pLeftHandItem = INVENTORY_GetLeftHandWeapon(pInventory);

		for (int i = 0; i < 11; ++i)
		{
			D2UnitStrc* pItem = pInventoryGrid->ppItems[i];
			if (pItem)
			{
				const int nWeaponClass = ITEMS_GetWeaponClassId(pInventoryGrid->ppItems[i]);
				if (nWeaponClass == WEAPONCLASS_1HS || nWeaponClass == WEAPONCLASS_1HT || nWeaponClass == WEAPONCLASS_HT1)
				{
					if (nComponent == COMPOSIT_LEFTHAND)
					{
						if (pItem != pLeftHandItem)
						{
							return pItem;
						}
					}
					else if (nComponent == COMPOSIT_RIGHTHAND && pItem == pLeftHandItem)
					{
						return pItem;
					}
				}
				else
				{
					if (ITEMS_GetComponent(pItem) == nComponent)
					{
						return pItem;
					}
				}
			}
		}
	}

	return nullptr;
}

//D2Common.0x6FD8FE80 (#10260)
// 1.13c D2Common+0x1E630: x position of the item when below 11, else 0; no delta
int __stdcall INVENTORY_GetBodyLocFromEquippedItem(D2InventoryStrc* pInventory, D2UnitStrc* pItem)
{
	if (INVENTORY_GetPtrIfValid(pInventory) && INVENTORY_UnitIsItem(pItem) && pItem->pStaticPath->tGameCoords.nX < 11)
	{
		return pItem->pStaticPath->tGameCoords.nX;
	}

	return 0;
}

//D2Common.0x6FD8FED0 (#11278)
// 1.13c D2Common+0x1E600
int __stdcall INVENTORY_GetItemsXPosition(D2InventoryStrc* pInventory, D2UnitStrc* pItem)
{
	if (INVENTORY_GetPtrIfValid(pInventory) && INVENTORY_UnitIsItem(pItem))
	{
		return pItem->pStaticPath->tGameCoords.nX;
	}

	return 0;
}

// The fields the 1.13c body (D2Common+0x21790) reads: inventory signature and cursor slot, unit type and item data, parent inventory.
static_assert(offsetof(D2InventoryStrc, dwSignature) == 0x00);
static_assert(offsetof(D2InventoryStrc, pCursorItem) == 0x20 + 5 * (sizeof(void*) - 4));
static_assert(offsetof(D2UnitStrc, dwUnitType) == 0x00);
static_assert(offsetof(D2UnitStrc, pItemData) == 0x14 + 1 * (sizeof(void*) - 4));
static_assert(offsetof(D2ItemDataStrc, pExtraData) == 0x5C);
static_assert(offsetof(D2ItemExtraDataStrc, pParentInv) == 0x00);

//D2Common.0x6FD8FF20 (#10261)
// 1.13c D2Common+0x21790: validated, no delta
void __stdcall INVENTORY_SetCursorItem(D2InventoryStrc* pInventory, D2UnitStrc* pItem)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		if (pItem)
		{
			D2ItemExtraDataStrc* pItemExtraData = INVENTORY_GetItemExtraDataFromItem(pItem);
			if (pItemExtraData)
			{
				pInventory->pCursorItem = pItem;
				pItemExtraData->pParentInv = pInventory;
			}
		}
		else
		{
			INVENTORY_RemoveItem(pInventory->pCursorItem);
		}
	}
}

//D2Common.0x6FD8FF80 (#10262)
// 1.13c D2Common+0x1DFB0
D2UnitStrc* __stdcall INVENTORY_GetCursorItem(D2InventoryStrc* pInventory)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		return pInventory->pCursorItem;
	}

	return nullptr;
}

// 1.13c D2Common+0x210b0: validated, no delta (grid from 0x1E1B0 with the player grid info, 0x26930 equality,
// quantity read inline from the stat list, 0x260F0 total max stack; the next-grid-item walk stops on a non-item).
//D2Common.0x6FD8FFA0 (#10263)
D2UnitStrc* __stdcall INVENTORY_FindBackPackItemForStack(D2InventoryStrc* pInventory, D2UnitStrc* pStackable, D2UnitStrc* pCheckItem)
{
	if (!INVENTORY_GetPtrIfValid(pInventory))
	{
		return nullptr;
	}

	D2InventoryGridStrc* pInventoryGrid = nullptr;
	if (pInventory->pOwner && pInventory->pOwner->dwUnitType == UNIT_PLAYER)
	{
		D2InventoryGridInfoStrc pInventoryGridInfo = {};
		DATATBLS_GetInventoryGridInfo(UNITS_GetInventoryRecordId(pInventory->pOwner, 0, TRUE), FALSE, &pInventoryGridInfo);
		pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_INVENTORY, &pInventoryGridInfo);
	}
	else
	{
		pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_INVENTORY, nullptr);
	}

	if (!pInventoryGrid)
	{
		return nullptr;
	}

	D2UnitStrc* pItem = pInventoryGrid->pItem;
	if (pCheckItem)
	{
		while (pItem)
		{
			if (pItem == pCheckItem)
			{
				break;
			}

			pItem = INVENTORY_GetNextGridItem(pItem);
		}
	}

	while (pItem)
	{
		if (ITEMS_AreStackablesEqual(pItem, pStackable) && STATLIST_UnitGetStatValue(pItem, STAT_QUANTITY, 0) < ITEMS_GetTotalMaxStack(pItem))
		{
			return pItem;
		}

		pItem = INVENTORY_GetNextGridItem(pItem);
	}

	return nullptr;
}

// 1.13c D2Common+0x20FD0
//D2Common.0x6FD90080 (#10264)
D2UnitStrc* __stdcall INVENTORY_FindEquippedItemForStack(D2InventoryStrc* pInventory, D2UnitStrc* pStackable, D2UnitStrc* pCheckItem)
{
	if (!INVENTORY_GetPtrIfValid(pInventory))
	{
		return nullptr;
	}

	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
	if (!pInventoryGrid)
	{
		return nullptr;
	}

	D2UnitStrc* pItem = pInventoryGrid->pItem;
	if (pCheckItem)
	{
		while (pItem)
		{
			if (pItem == pCheckItem)
			{
				break;
			}

			pItem = INVENTORY_GetNextGridItem(pItem);
		}
	}

	while (pItem)
	{
		if (ITEMS_AreStackablesEqual(pItem, pStackable) && STATLIST_UnitGetStatValue(pItem, STAT_QUANTITY, 0) < ITEMS_GetTotalMaxStack(pItem))
		{
			return pItem;
		}

		pItem = INVENTORY_GetNextGridItem(pItem);
	}

	return nullptr;
}

//D2Common.0x6FD90130 (#10265)
// 1.13c D2Common+0x20BC0: owner player picks the layout record, pack grid chain from the check item, book with the same suffix id and room left; no delta
D2UnitStrc* __stdcall INVENTORY_FindFillableBook(D2InventoryStrc* pInventory, D2UnitStrc* pScrolls, D2UnitStrc* pCheckItem)
{
	if (!INVENTORY_GetPtrIfValid(pInventory))
	{
		return nullptr;
	}

	D2InventoryGridStrc* pInventoryGrid = nullptr;
	if (pInventory->pOwner && pInventory->pOwner->dwUnitType == UNIT_PLAYER)
	{
		D2InventoryGridInfoStrc pInventoryGridInfo = {};
		DATATBLS_GetInventoryGridInfo(UNITS_GetInventoryRecordId(pInventory->pOwner, INVPAGE_INVENTORY, TRUE), FALSE, &pInventoryGridInfo);
		pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_INVENTORY, &pInventoryGridInfo);
	}
	else
	{
		pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_INVENTORY, nullptr);
	}

	if (!pInventoryGrid)
	{
		return nullptr;
	}

	D2UnitStrc* pItem = pInventoryGrid->pItem;
	if (pCheckItem)
	{
		while (pItem)
		{
			if (pItem == pCheckItem)
			{
				break;
			}

			pItem = INVENTORY_GetNextGridItem(pItem);
		}
	}

	while (pItem)
	{
		if (ITEMS_GetItemType(pItem) == ITEMTYPE_BOOK && ITEMS_GetSuffixId(pItem, 0) == ITEMS_GetSuffixId(pScrolls, 0) && STATLIST_UnitGetStatValue(pItem, STAT_QUANTITY, 0) < ITEMS_GetTotalMaxStack(pItem))
		{
			return pItem;
		}

		pItem = INVENTORY_GetNextGridItem(pItem);
	}

	return nullptr;
}

//D2Common.0x6FD90230 (#10266)
// 1.13c D2Common+0x21EE0
BOOL __stdcall INVENTORY_PlaceItemInBeltSlot(D2InventoryStrc* pInventory, D2UnitStrc* pItem, int nSlot)
{
	if (INVENTORY_UnitIsItem(pItem) && ITEMS_CheckIfBeltable(pItem))
	{
		uint8_t nHeight = 0;
		uint8_t nWidth = 0;

		ITEMS_GetDimensions(pItem, &nWidth, &nHeight, __FILE__, __LINE__);

		if (nWidth == 1 && nHeight == 1 && pInventory && nSlot >= 0 && nSlot < 16)
		{
			return INVENTORY_PlaceItemInGrid(pInventory, pItem, nSlot, 0, INVGRID_BELT, 0, 0);
		}
	}

	return FALSE;
}

//D2Common.0x6FD902B0 (#10268)
// 1.13c D2Common+0x1EEE0: not isc/tsc, any of the four belt slots with a matching potion type; no delta
BOOL __stdcall INVENTORY_HasSimilarPotionInBelt(D2InventoryStrc* pInventory, D2UnitStrc* pPotion)
{
	if (INVENTORY_GetPtrIfValid(pInventory) && INVENTORY_UnitIsItem(pPotion))
	{
		D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pPotion->dwClassId);
		if (pItemsTxtRecord && pItemsTxtRecord->dwCode != ' csi' && pItemsTxtRecord->dwCode != ' cst')
		{
			D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BELT, &gBeltInventoryGridInfo);
			if (pInventoryGrid)
			{
				for (int i = 0; i < 4; ++i)
				{
					if (pInventoryGrid->ppItems[i] && ITEMS_ComparePotionTypes(pPotion, pInventoryGrid->ppItems[i]))
					{
						return TRUE;
					}
				}
			}
		}
	}
	
	return FALSE;
}

//D2Common.0x6FD90340 (#10269)
// 1.13c D2Common+0x200D0
BOOL __stdcall INVENTORY_GetFreeBeltSlot(D2InventoryStrc* pInventory, D2UnitStrc* pItem, int* pFreeSlotId)
{
	if (!INVENTORY_GetPtrIfValid(pInventory) || !INVENTORY_UnitIsItem(pItem) || !ITEMS_CheckIfBeltable(pItem))
	{
		return FALSE;
	}

	uint8_t nHeight = 0;
	uint8_t nWidth = 0;
	ITEMS_GetDimensions(pItem, &nWidth, &nHeight, __FILE__, __LINE__);
	if (nWidth != 1 || nHeight != 1)
	{
		return FALSE;
	}

	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
	D2UnitStrc* pBelt = nullptr;
	if (pInventoryGrid)
	{
		pBelt = pInventoryGrid->ppItems[BODYLOC_BELT];
	}

	int nBeltType = 2;
	if (pBelt)
	{
		nBeltType = UNITS_GetBeltType(pBelt);
	}

	D2BeltsTxt pBeltsTxtRecord = {};
	DATATBLS_GetBeltsTxtRecord(nBeltType, 0, &pBeltsTxtRecord);

	pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BELT, &gBeltInventoryGridInfo);
	if (pInventoryGrid)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (pInventoryGrid->ppItems[i] && ITEMS_ComparePotionTypes(pItem, pInventoryGrid->ppItems[i]))
			{
				for (int nId = i; nId < pBeltsTxtRecord.nBoxes; nId += 4)
				{
					if (!pInventoryGrid->ppItems[nId])
					{
						*pFreeSlotId = nId;
						return TRUE;
					}
				}
			}
		}

		D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		if (!pItemsTxtRecord || !pItemsTxtRecord->nAutoBelt)
		{
			return FALSE;
		}

		for (int i = 0; i < 4; ++i)
		{
			if (!pInventoryGrid->ppItems[i])
			{
				*pFreeSlotId = i;
				return TRUE;
			}
		}
	}

	return FALSE;
}

//D2Common.0x6FD904F0 (#10270)
// 1.13c D2Common+0x21E80
BOOL __stdcall INVENTORY_PlaceItemInFreeBeltSlot(D2InventoryStrc* pInventory, D2UnitStrc* pItem)
{
	int nFreeSlot = 0;
	if (INVENTORY_GetFreeBeltSlot(pInventory, pItem, &nFreeSlot) && pInventory && INVENTORY_UnitIsItem(pItem) && nFreeSlot >= 0 && nFreeSlot < 16)
	{
		return INVENTORY_PlaceItemInGrid(pInventory, pItem, nFreeSlot, 0, INVGRID_BELT, 0, 0);
	}

	return FALSE;
}

// 1.13c D2Common+0x1E5B0
//D2Common.0x6FD90550 (#10271)
D2UnitStrc* __stdcall INVENTORY_GetItemFromBeltSlot(D2InventoryStrc* pInventory, int nSlotId)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BELT, &gBeltInventoryGridInfo);
		if (pInventoryGrid && nSlotId >= 0 && nSlotId < 16)
		{
			return pInventoryGrid->ppItems[nSlotId];
		}
	}

	return nullptr;
}

//D2Common.0x6FD90590 (#10272)
// 1.13c D2Common+0x20010
BOOL __stdcall INVENTORY_GetUseableItemFromBeltSlot(D2InventoryStrc* pInventory, D2UnitStrc* pItem, int nSlotId, D2UnitStrc** ppItem)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BELT, &gBeltInventoryGridInfo);
		if (pInventoryGrid && INVENTORY_UnitIsItem(pItem) && ITEMS_CheckIfBeltable(pItem))
		{
			uint8_t nHeight = 0;
			uint8_t nWidth = 0;
			ITEMS_GetDimensions(pItem, &nWidth, &nHeight, __FILE__, __LINE__);

			if (nWidth == 1 && nHeight == 1)
			{
				D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
				if (pItemsTxtRecord && pItemsTxtRecord->nUseable == 1 && pItemsTxtRecord->nBelt == 1)
				{
					if (nSlotId >= 0 && nSlotId < 16)
					{
						*ppItem = pInventoryGrid->ppItems[nSlotId];
					}
					else
					{
						*ppItem = nullptr;
					}

					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

//D2Common.0x6FD90690 (#10273)
// 1.13c D2Common+0x1FF40: a null inventory returns before the out pointer is cleared; a bad signature clears it and finds nothing
BOOL __stdcall INVENTORY_GetEquippedShield(D2InventoryStrc* pInventory, D2UnitStrc** ppItem)
{
	if (!pInventory)
	{
		return FALSE;
	}

	if (ppItem)
	{
		*ppItem = nullptr;
	}

	if (!INVENTORY_GetPtrIfValid(pInventory))
	{
		return FALSE;
	}

	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
	D2UnitStrc* pItem = nullptr;
	if (pInventoryGrid)
	{
		pItem = pInventoryGrid->ppItems[BODYLOC_RARM];
	}
	else
	{
		pItem = nullptr;
	}

	if (ITEMS_CanBeEquipped(pItem) && ITEMS_CheckItemTypeId(pItem, ITEMTYPE_ANY_SHIELD))
	{
		if (ppItem)
		{
			*ppItem = pItem;
		}
		return TRUE;
	}

	if (pInventoryGrid)
	{
		pItem = pInventoryGrid->ppItems[BODYLOC_LARM];
	}
	else
	{
		pItem = nullptr;
	}

	if (ITEMS_CanBeEquipped(pItem) && ITEMS_CheckItemTypeId(pItem, ITEMTYPE_ANY_SHIELD))
	{
		if (ppItem)
		{
			*ppItem = pItem;
		}
		return TRUE;
	}

	return FALSE;
}

//D2Common.0x6FD90760 (#10274)
// 1.13c D2Common+0x20700
BOOL __stdcall INVENTORY_GetEquippedWeapon(D2InventoryStrc* pInventory, D2UnitStrc** ppItem, int* pBodyLoc, BOOL* pIsLeftHandItem)
{
	// 1.13c: a null inventory writes nothing; a bad signature clears the left-hand flag and the item, and skips the grid
	if (!pInventory)
	{
		return FALSE;
	}

	*pIsLeftHandItem = FALSE;

	D2UnitStrc* pLeftHandItem = INVENTORY_GetLeftHandWeapon(pInventory);

	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetPtrIfValid(pInventory) ? INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo) : nullptr;

	D2UnitStrc* pItem = pInventoryGrid ? pInventoryGrid->ppItems[BODYLOC_RARM] : nullptr;

	if (ITEMS_CanBeEquipped(pItem) && ITEMS_CheckItemTypeId(pItem, ITEMTYPE_WEAPON))
	{
		*ppItem = pItem;
		*pBodyLoc = BODYLOC_RARM;

		if (pItem == pLeftHandItem)
		{
			*pIsLeftHandItem = TRUE;
		}

		return TRUE;
	}
	else
	{
		pItem = pInventoryGrid ? pInventoryGrid->ppItems[BODYLOC_LARM] : nullptr;

		if (ITEMS_CanBeEquipped(pItem) && ITEMS_CheckItemTypeId(pItem, ITEMTYPE_WEAPON))
		{
			*ppItem = pItem;
			*pBodyLoc = BODYLOC_LARM;

			if (pItem == pLeftHandItem)
			{
				*pIsLeftHandItem = TRUE;
			}

			return TRUE;
		}
	}

	*ppItem = nullptr;
	return FALSE;
}

//D2Common.0x6FD90850 (#10275)
// 1.13c D2Common+0x1FEF0
BOOL __stdcall INVENTORY_HasBodyArmorEquipped(D2InventoryStrc* pInventory)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
		if (pInventoryGrid && pInventoryGrid->ppItems[BODYLOC_TORSO])
		{
			return ITEMS_CheckItemTypeId(pInventoryGrid->ppItems[BODYLOC_TORSO], ITEMTYPE_ARMOR) != 0;
		}
	}

	return FALSE;
}

//D2Common.0x6FD908A0 (#10276)
// 1.13c D2Common+0x20B50: validated, no delta (body slot occupied and a free position exists for the occupant)
BOOL __stdcall INVENTORY_IsItemBodyLocFree(D2InventoryStrc* pInventory, D2UnitStrc* pItem, int nBodyLoc, int nInventoryRecordId)
{
	if (INVENTORY_GetPtrIfValid(pInventory) && INVENTORY_ValidateBodyLoc(nBodyLoc))
	{
		D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);

		if (pInventoryGrid && pInventoryGrid->ppItems[nBodyLoc] && pItem)
		{
			int nFreeX = 0;
			int nFreeY = 0;
			return INVENTORY_GetFreePosition(pInventory, pInventoryGrid->ppItems[nBodyLoc], nInventoryRecordId, &nFreeX, &nFreeY, 0) != 0;
		}
	}

	return FALSE;
}

//D2Common.0x6FD90910 (#10279)
// 1.13c D2Common+0x21760
void __stdcall INVENTORY_RemoveInventoryItems(D2InventoryStrc* pInventory)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		for (D2UnitStrc* pItem = pInventory->pFirstItem; pItem; pItem = pInventory->pFirstItem)
		{
			INVENTORY_RemoveItem(pItem);
		}
	}
}

//D2Common.0x6FD90940 (#10280)
// 1.13c D2Common+0x1DF70
D2InventoryNodeStrc* __stdcall INVENTORY_GetTradeInventory(D2InventoryStrc* pInventory)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		return pInventory->pFirstNode;
	}

	return nullptr;
}

//D2Common.0x6FD90960 (#10281)
// 1.13c D2Common+0x1DF20
void __stdcall INVENTORY_FreeTradeInventory(D2InventoryStrc* pInventory)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		D2InventoryNodeStrc* pNextNode = nullptr;
		for (D2InventoryNodeStrc* pNode = pInventory->pFirstNode; pNode; pNode = pNextNode)
		{
			pNextNode = pNode->pNext;
			D2_FREE_POOL(pInventory->pMemPool, pNode);
		}

		pInventory->pFirstNode = nullptr;
		pInventory->pLastNode = nullptr;
	}
}

//D2Common.0x6FD909B0 (#10282)
// 1.13c D2Common+0x1DEE0
BOOL __stdcall INVENTORY_CheckForItemInTradeInventory(D2InventoryStrc* pInventory, int nItemId)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		for (D2InventoryNodeStrc* pNode = pInventory->pFirstNode; pNode; pNode = pNode->pNext)
		{
			if (pNode->nItemId == nItemId)
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

//D2Common.0x6FD909F0 (#10283)
// 1.13c D2Common+0x1E4F0
void __stdcall INVENTORY_AddItemToTradeInventory(D2InventoryStrc* pInventory, D2UnitStrc* pItem)
{
	if (INVENTORY_UnitIsItem(pItem))
	{
		if (INVENTORY_CheckForItemInTradeInventory(pInventory, pItem->dwUnitId))
		{
			return;
		}

		D2InventoryNodeStrc* pNode = D2_ALLOC_STRC_POOL(pInventory->pMemPool, D2InventoryNodeStrc);
		D2_ASSERT(pNode);

		pNode->pNext = nullptr;
		pNode->nItemId = pItem->dwUnitId;

		if (pInventory->pFirstNode)
		{
			pInventory->pLastNode->pNext = pNode;
		}
		else
		{
			pInventory->pFirstNode = pNode;
		}

		pInventory->pLastNode = pNode;
	}
}

//D2Common.0x6FD90AB0 (#10316)
// 1.13c D2Common+0x1DC40: null check, dword at +0x00 (no trace in retail)
int __stdcall D2Common_10316(D2CorpseStrc* pCorpse)
{
	if (pCorpse)
	{
		return pCorpse->unk0x00;
	}

	return 0;
}

//D2Common.0x6FD90AC0 (#10284)
// 1.13c D2Common+0x1DE90: signature check, dword at +0x28; no delta
int __stdcall INVENTORY_GetItemCount(D2InventoryStrc* pInventory)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		return pInventory->dwItemCount;
	}

	return 0;
}

//D2Common.0x6FD90AE0 (#10285)
// 1.13c D2Common+0x1FE10
D2UnitStrc* __stdcall INVENTORY_GetBackPackItemByType(D2InventoryStrc* pInventory, int nItemType, D2UnitStrc* pCheckItem)
{
	if (!INVENTORY_GetPtrIfValid(pInventory))
	{
		return nullptr;
	}

	D2InventoryGridStrc* pInventoryGrid = nullptr;
	if (pInventory->pOwner && pInventory->pOwner->dwUnitType == UNIT_PLAYER)
	{
		D2InventoryGridInfoStrc pInventoryGridInfo = {};
		DATATBLS_GetInventoryGridInfo(UNITS_GetInventoryRecordId(pInventory->pOwner, 0, 1), 0, &pInventoryGridInfo);
		pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_INVENTORY, &pInventoryGridInfo);
	}
	else
	{
		pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_INVENTORY, nullptr);
	}
	
	if (!pInventoryGrid)
	{
		return nullptr;
	}

	D2UnitStrc* pItem = pInventoryGrid->pItem;
	if (pCheckItem)
	{
		while (pItem)
		{
			if (pItem == pCheckItem)
			{
				break;
			}

			pItem = INVENTORY_GetNextGridItem(pItem);
		}

		pItem = INVENTORY_GetNextGridItem(pItem);
	}

	while (pItem)
	{
		if (ITEMS_CheckItemTypeId(pItem, nItemType))
		{
			return pItem;
		}

		pItem = INVENTORY_GetNextGridItem(pItem);
	}

	return nullptr;
}

//D2Common.0x6FD90BC0 (#10286)
// 1.13c D2Common+0x1FD40: body grid chain from the check item on, first item of the type with x < 11; no delta
D2UnitStrc* __stdcall INVENTORY_GetEquippedItemByType(D2InventoryStrc* pInventory, int nItemType, D2UnitStrc* pCheckItem)
{
	if (!INVENTORY_GetPtrIfValid(pInventory))
	{
		return nullptr;
	}

	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
	if (!pInventoryGrid)
	{
		return nullptr;
	}

	D2UnitStrc* pItem = pInventoryGrid->pItem;
	if (pCheckItem)
	{
		while (pItem)
		{
			if (pItem == pCheckItem)
			{
				break;
			}

			pItem = INVENTORY_GetNextGridItem(pItem);
		}
	}

	while (pItem)
	{
		if (ITEMS_CheckItemTypeId(pItem, nItemType) && UNITS_GetXPosition(pItem) < 11)
		{
			return pItem;
		}

		pItem = INVENTORY_GetNextGridItem(pItem);
	}

	return nullptr;
}

//D2Common.0x6FD90C80 (#10287)
// 1.13c D2Common+0x1EDF0: item code to class id, body grid chain, first item of the class with x < 11; no delta
D2UnitStrc* __stdcall INVENTORY_GetEquippedItemByCode(D2InventoryStrc* pInventory, int nItemCode, D2UnitStrc* pCheckItem)
{
	int nClassId = 0;
	if (!INVENTORY_GetPtrIfValid(pInventory) || !DATATBLS_GetItemRecordFromItemCode(nItemCode, &nClassId))
	{
		return nullptr;
	}

	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
	if (!pInventoryGrid)
	{
		return nullptr;
	}

	D2UnitStrc* pItem = pInventoryGrid->pItem;
	if (pCheckItem)
	{
		while (pItem)
		{
			if (pItem == pCheckItem)
			{
				break;
			}

			pItem = INVENTORY_GetNextGridItem(pItem);
		}
	}

	while (pItem)
	{
		if (pItem->dwClassId == nClassId && UNITS_GetXPosition(pItem) < 11)
		{
			return pItem;
		}

		pItem = INVENTORY_GetNextGridItem(pItem);
	}

	return nullptr;
}

//D2Common.0x6FD90D50 (#11306)
// 1.13c D2Common+0x1ED00: as 0x1EDF0 on grid 2 (retail passes the same 13x1 size pair for it); no delta
D2UnitStrc* __stdcall INVENTORY_GetBackPackItemByCode(D2InventoryStrc* pInventory, int nItemCode, D2UnitStrc* pCheckItem)
{
	int nClassId = 0;
	if (!INVENTORY_GetPtrIfValid(pInventory) || !DATATBLS_GetItemRecordFromItemCode(nItemCode, &nClassId))
	{
		return nullptr;
	}
	
	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_INVENTORY, &gBodyLocInventoryGridInfo);
	if (!pInventoryGrid)
	{
		return nullptr;
	}

	D2UnitStrc* pItem = pInventoryGrid->pItem;
	if (pCheckItem)
	{
		while (pItem)
		{
			if (pItem == pCheckItem)
			{
				break;
			}

			pItem = INVENTORY_GetNextGridItem(pItem);
		}
	}

	while (pItem)
	{
		if (pItem->dwClassId == nClassId && UNITS_GetXPosition(pItem) < 11)
		{
			return pItem;
		}

		pItem = INVENTORY_GetNextGridItem(pItem);
	}

	return nullptr;
}

//D2Common.0x6FD90E20 (#10288)
// 1.13c D2Common+0x1EC30: body grid chain, set-quality items of the file index with x < 11; no delta
int __stdcall INVENTORY_GetSetItemEquipCountByFileIndex(D2InventoryStrc* pInventory, int nItemFileIndex)
{
	if (!INVENTORY_GetPtrIfValid(pInventory))
	{
		return 0;
	}

	D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
	if (pInventoryGrid)
	{
		int nCounter = 0;
		for (D2UnitStrc* pItem = pInventoryGrid->pItem; ITEMS_GetItemData(pItem) != nullptr; pItem = pItem->pItemData->pExtraData.pNextGridItem)
		{
			if (ITEMS_GetItemQuality(pItem) == ITEMQUAL_SET && ITEMS_GetFileIndex(pItem) == nItemFileIndex)
			{
				if (pItem->pStaticPath->tGameCoords.nX < 11)
				{
					++nCounter;
				}
			}
		}

		return nCounter;
	}

	return 0;
}

//D2Common.0x6FD90ED0 (#10289)
// 1.13c D2Common+0x20620
void __stdcall INVENTORY_UpdateWeaponGUIDOnInsert(D2InventoryStrc* pInventory, D2UnitStrc* pItem)
{
	if (pInventory && INVENTORY_UnitIsItem(pItem) && ITEMS_CheckItemTypeId(pItem, ITEMTYPE_WEAPON))
	{
		if (ITEMS_GetBodyLocation(pItem) == BODYLOC_RARM || ITEMS_GetBodyLocation(pItem) == BODYLOC_LARM)
		{
			if (ITEMS_CanBeEquipped(pItem))
			{
				D2UnitStrc* pWeapon = INVENTORY_GetLeftHandWeapon(pInventory);
				if (!pWeapon || !ITEMS_CheckItemTypeId(pWeapon, ITEMTYPE_WEAPON) || ITEMS_CheckItemTypeId(pWeapon, ITEMTYPE_MISSILE_POTION))
				{
					if (INVENTORY_CheckSignature(pInventory))
					{
						pInventory->dwLeftItemGUID = pItem->dwUnitId;
					}
					return;
				}
			}
			
			if (INVENTORY_CheckSignature(pInventory) && pInventory->dwLeftItemGUID == pItem->dwUnitId)
			{
				pInventory->dwLeftItemGUID = D2UnitInvalidGUID;
			}
		}
	}
}

//D2Common.0x6FD90F80 (#10290)
// 1.13c D2Common+0x1FC70
void __stdcall INVENTORY_UpdateWeaponGUIDOnRemoval(D2InventoryStrc* pInventory, D2UnitStrc* pItem)
{
	if (INVENTORY_GetPtrIfValid(pInventory) && INVENTORY_UnitIsItem(pItem) && ITEMS_CheckItemTypeId(pItem, ITEMTYPE_WEAPON))
	{
		if (pInventory->dwLeftItemGUID == pItem->dwUnitId)
		{
			pInventory->dwLeftItemGUID = D2UnitInvalidGUID;
		}

		D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
		if (pInventoryGrid)
		{
			const int nBodyLoc = ITEMS_GetBodyLocation(pItem);
			D2UnitStrc* pWeapon = nullptr;
			if (nBodyLoc == BODYLOC_RARM)
			{
				pWeapon = pInventoryGrid->ppItems[BODYLOC_LARM];
			}
			else if (nBodyLoc == BODYLOC_LARM)
			{
				pWeapon = pInventoryGrid->ppItems[BODYLOC_RARM];
			}
			else
			{
				return;
			}

			if (pWeapon && ITEMS_CanBeEquipped(pWeapon) && pWeapon->dwUnitType == UNIT_ITEM)
			{
				pInventory->dwLeftItemGUID = pWeapon->dwUnitId;
			}
		}
	}
}

//D2Common.0x6FD91050 (#10291)
// 1.13c D2Common+0x1FB80
int __stdcall INVENTORY_GetWieldType(D2UnitStrc* pPlayer, D2InventoryStrc* pInventory)
{
	if (pInventory)
	{
		D2UnitStrc* pRightHandItem = nullptr;
		D2UnitStrc* pLeftHandItem = nullptr;

		if (INVENTORY_CheckSignature(pInventory))
		{
			D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
			if (pInventoryGrid)
			{
				pLeftHandItem = pInventoryGrid->ppItems[BODYLOC_LARM];
				pRightHandItem = pInventoryGrid->ppItems[BODYLOC_RARM];
			}
		}

		if (!pLeftHandItem)
		{
			if (!pRightHandItem)
			{
				return -1;
			}

			if (ITEMS_CheckWeaponIfTwoHanded(pRightHandItem))
			{
				return 2;
			}

			if (ITEMS_Is1Or2Handed(pPlayer, pRightHandItem))
			{
				return 2;
			}
			else
			{
				return -1;
			}
		}

		if (!pRightHandItem)
		{
			if (ITEMS_CheckWeaponIfTwoHanded(pLeftHandItem))
			{
				return 2;
			}

			if (ITEMS_Is1Or2Handed(pPlayer, pLeftHandItem))
			{
				return 2;
			}
			else
			{
				return -1;
			}
		}

		if (ITEMS_Is1Or2Handed(pPlayer, pLeftHandItem) || ITEMS_Is1Or2Handed(pPlayer, pRightHandItem))
		{
			return 1;
		}

		if (ITEMS_CheckWeaponIfTwoHanded(pLeftHandItem))
		{
			return 2;
		}

		if (ITEMS_CheckWeaponIfTwoHanded(pRightHandItem))
		{
			return 2;
		}
		else
		{
			return -1;
		}
	}

	return -1;
}

// 1.13c D2Common+0x1DE70
//D2Common.0x6FD91140 (#10292)
void __stdcall INVENTORY_SetOwnerId(D2InventoryStrc* pInventory, D2UnitGUID nOwnerGuid)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		pInventory->dwOwnerGuid = nOwnerGuid;
	}
}

//D2Common.0x6FD91160 (#10293)
// 1.13c D2Common+0x1DE50
int __stdcall INVENTORY_GetOwnerId(D2InventoryStrc* pInventory)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		return pInventory->dwOwnerGuid;
	}

	return -1;
}

//D2Common.0x6FD91190 (#10294)
// 1.13c D2Common+0x1DDD0
void __stdcall INVENTORY_CreateCorpseForPlayer(D2InventoryStrc* pInventory, int nUnitId, int a3, int a4)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		D2CorpseStrc* pCorpse = D2_ALLOC_STRC_POOL(pInventory->pMemPool, D2CorpseStrc);
		pCorpse->unk0x00 = a4;
		pCorpse->dwUnitId = nUnitId;
		pCorpse->unk0x08 = a3;
		pCorpse->pNextCorpse = nullptr;

		if (pInventory->pFirstCorpse)
		{
			pInventory->pLastCorpse->pNextCorpse = pCorpse;
		}
		else
		{
			pInventory->pFirstCorpse = pCorpse;
		}

		pInventory->pLastCorpse = pCorpse;

		if (pCorpse->unk0x00)
		{
			++pInventory->nCorpseCount;
		}
	}
}

//D2Common.0x6FD91210 (#10295)
// 1.13c D2Common+0x1DD40
BOOL __stdcall INVENTORY_FreeCorpse(D2InventoryStrc* pInventory, int nUnitId, int a3)
{
	if (INVENTORY_GetPtrIfValid(pInventory) && pInventory->pFirstCorpse)
	{
		D2CorpseStrc* pPreviousCorpse = nullptr;
		D2CorpseStrc* pCorpse = pInventory->pFirstCorpse;
		while (pCorpse->dwUnitId != nUnitId || pCorpse->unk0x00 != a3)
		{
			pPreviousCorpse = pCorpse;

			pCorpse = pCorpse->pNextCorpse;
			if (!pCorpse)
			{
				return FALSE;
			}
		}

		if (pPreviousCorpse)
		{
			pPreviousCorpse->pNextCorpse = pCorpse->pNextCorpse;
			if (!pCorpse->pNextCorpse)
			{
				pInventory->pLastCorpse = pPreviousCorpse;
			}
		}
		else
		{
			pInventory->pFirstCorpse = pCorpse->pNextCorpse;
			if (!pCorpse->pNextCorpse)
			{
				pInventory->pLastCorpse = nullptr;
			}
		}

		if (pCorpse->unk0x00)
		{
			--pInventory->nCorpseCount;
		}

		D2_FREE_POOL(pInventory->pMemPool, pCorpse);
		return TRUE;
	}
	
	return FALSE;
}

//D2Common.0x6FD91290 (#10296)
// 1.13c D2Common+0x1DD20
D2CorpseStrc* __stdcall INVENTORY_GetFirstCorpse(D2InventoryStrc* pInventory)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		return pInventory->pFirstCorpse;
	}

	return nullptr;
}

//D2Common.0x6FD912B0 (#10297)
// 1.13c D2Common+0x1DCB0: signature check, dword at +0x3C; no delta
int __stdcall INVENTORY_GetCorpseCount(D2InventoryStrc* pInventory)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		return pInventory->nCorpseCount;
	}

	return 0;
}

//D2Common.0x6FD912D0 (#10313)
// 1.13c D2Common+0x1DC90: null check, next pointer at +0x0C (no trace in retail)
D2CorpseStrc* __stdcall INVENTORY_GetNextCorpse(D2CorpseStrc* pCorpse)
{
	if (pCorpse)
	{
		return pCorpse->pNextCorpse;
	}

	return nullptr;
}

//D2Common.0x6FDAFEA0 (#10314)
// 1.13c D2Common+0x1DC70: null check, dword at +0x04 (no trace in retail)
D2UnitGUID __stdcall INVENTORY_GetUnitGUIDFromCorpse(D2CorpseStrc* pCorpse)
{
	if (pCorpse)
	{
		return pCorpse->dwUnitId;
	}

	return 0;
}

//D2Common.0x6FDB18D0 (#10315)
// 1.13c D2Common+0x1DC50: null check, dword at +0x08
int __stdcall D2Common_10315(D2CorpseStrc* pCorpse)
{
	if (pCorpse)
	{
		return pCorpse->unk0x08; // Used as merc name id?
	}

	return 0;
}

void __fastcall INVENTORY_InitializeComponentArray();

// 1.13c D2Common+0x1FB50: the slot search over 1..254 for the alternate graphic or the code; 0 when no slot matches
inline int INVENTORY_GetComponentArrayIndexFromItemsTxtRecord(D2ItemsTxt* pItemsTxtRecord)
{
	INVENTORY_InitializeComponentArray();   // 1.13c D2Common+0x1FB50 builds the table first
	for (int nCounter = 1; nCounter < kComponentArrayLogicalSize; ++nCounter)
	{
		if (gTxtComponentItemTypeMap[nCounter].dwCode == pItemsTxtRecord->dwAlternateGfx || gTxtComponentItemTypeMap[nCounter].dwCode == pItemsTxtRecord->dwCode)
		{
			return nCounter;
		}
	}

	return 0;
}

// 1.13c D2Common+0x1FB50: the same search with the arm type token as both keys; 0 when no slot matches
inline int INVENTORY_GetComponentArrayIndexFromArmTypeTxtRecord(D2ArmTypeTxt* pArmTypeTxtRecord)
{
	INVENTORY_InitializeComponentArray();   // 1.13c D2Common+0x1FB50 builds the table first
	for (int nCounter = 1; nCounter < kComponentArrayLogicalSize; ++nCounter)
	{
		if (gTxtComponentItemTypeMap[nCounter].dwCode == *(uint32_t*)&pArmTypeTxtRecord->szToken[0])
		{
			return nCounter;
		}
	}

	return 0;
}

// 1.13c D2Common+0x21300: head and hand items. Slot from the items.txt component byte, forced to 5/6 for the composite right/left hand item.
static void INVENTORY_UpdateEquippedItemComponent(D2UnitStrc* pPlayer, uint8_t* pComponents, uint8_t* pColor, D2UnitStrc* pItem)
{
	D2InventoryStrc* pInventory = pPlayer->pInventory;
	if (!pInventory)
	{
		return;
	}

	D2UnitStrc* pRightHandItem = INVENTORY_GetCompositItem(pInventory, COMPOSIT_RIGHTHAND);
	D2UnitStrc* pLeftHandItem = INVENTORY_GetCompositItem(pInventory, COMPOSIT_LEFTHAND);

	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);

	uint8_t nComponent = ITEMS_GetComponent(pItem);
	const int nIndex = INVENTORY_GetComponentArrayIndexFromItemsTxtRecord(pItemsTxtRecord);
	if (nIndex == 0)
	{
		if (nComponent < NUM_COMPONENTS)
		{
			pComponents[nComponent] = -1;
			pColor[nComponent] = -1;
		}
		return;
	}

	if (pItem == pRightHandItem)
	{
		nComponent = COMPOSIT_RIGHTHAND;
	}
	else if (pItem == pLeftHandItem)
	{
		nComponent = COMPOSIT_LEFTHAND;
	}
	else if (nComponent >= NUM_COMPONENTS)
	{
		return;
	}

	pComponents[nComponent] = nIndex;

	sub_6FD917B0(pPlayer, pComponents, pColor, pItem);

	uint8_t* pComponentColor = &pColor[nComponent];
	if (ITEMS_GetColor(pPlayer, pItem, pComponentColor, 0))
	{
		++*pComponentColor;
	}
	else
	{
		*pComponentColor = -1;
	}
}

//D2Common.0x6FD912F0 (#10298)
// 1.13c D2Common+0x21420: the walk stops at the first non-item node; item data +0x44 (body location) 1/4/5 go to the head and hand update, 3 fills the six armor slots
void __stdcall INVENTORY_GetItemSaveGfxInfo(D2UnitStrc* pPlayer, uint8_t* pComponents, uint8_t* pColor)
{
	for (D2UnitStrc* pItem = INVENTORY_GetFirstItem(pPlayer->pInventory); pItem; pItem = INVENTORY_GetNextItem(pItem))
	{
		if (pItem->dwUnitType == UNIT_ITEM && pItem->dwAnimMode == IMODE_EQUIP)
		{
			D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
			D2_ASSERT(pItemsTxtRecord);

			if (D2ItemDataStrc* pItemData = pItem->pItemData)
			{
				const uint8_t nBodyLoc = pItemData->nBodyLoc;
				if (nBodyLoc == BODYLOC_HEAD || nBodyLoc == BODYLOC_RARM || nBodyLoc == BODYLOC_LARM)
				{
					if (!ITEMS_CheckType(pItemsTxtRecord->wType[0], ITEMTYPE_CIRCLET))
					{
						INVENTORY_UpdateEquippedItemComponent(pPlayer, pComponents, pColor, pItem);
					}
				}
				else if (nBodyLoc == BODYLOC_TORSO)
				{
					uint8_t pArmorComponents[6] = {};
					for (int i = 0; i < 6; ++i)
					{
						pArmorComponents[i] = pItemsTxtRecord->nArmorComp[i];
					}

					for (int i = 0; i < NUM_COMPONENTS; ++i)
					{
						if (COMPOSIT_IsArmorComponent(i))
						{
							D2ArmTypeTxt* pArmTypeTxtRecord = DATATBLS_GetArmTypeTxtRecord(COMPOSIT_GetArmorTypeFromComponent(i, pArmorComponents));
							D2_ASSERT(pArmTypeTxtRecord);

							int nIndex = 0;
							if (pItem->dwClassId > 0)
							{
								nIndex = INVENTORY_GetComponentArrayIndexFromArmTypeTxtRecord(pArmTypeTxtRecord);
							}

							if (nIndex == 0)
							{
								pComponents[i] = -1;
								pColor[i] = -1;
							}
							else
							{
								pComponents[i] = nIndex;

								uint8_t* pComponentColor = &pColor[i];
								if (ITEMS_GetColor(pPlayer, pItem, pComponentColor, 0))
								{
									++*pComponentColor;
								}
								else
								{
									*pComponentColor = -1;
								}
							}
						}
					}
				}
			}
		}
	}
}

//D2Common.0x6FD915C0
// 1.13c D2Common+0x1E920: slots 1..3 preset; a record joins when it is a weapon, armor, shield or helm and is not a circlet
void __fastcall INVENTORY_InitializeComponentArray()
{
	if (!gbComponentArrayInitialized)
	{
		gbComponentArrayInitialized = TRUE;

		memset(gTxtComponentItemTypeMap, 0x00, sizeof(gTxtComponentItemTypeMap));

		gTxtComponentItemTypeMap[1].dwCode = ' til';
		gTxtComponentItemTypeMap[1].nItemType = ITEMTYPE_ARMOR;
		gTxtComponentItemTypeMap[2].dwCode = ' dem';
		gTxtComponentItemTypeMap[2].nItemType = ITEMTYPE_ARMOR;
		gTxtComponentItemTypeMap[3].dwCode = ' yvh';
		gTxtComponentItemTypeMap[3].nItemType = ITEMTYPE_ARMOR;

		D2ItemDataTbl* pItemDataTbl = DATATBLS_GetItemDataTables();

		int nCurrentTableEntries = 4;

		for (int i = 0; i < pItemDataTbl->nItemsTxtRecordCount; ++i)
		{
			D2ItemsTxt* pItemsTxtRecord = &pItemDataTbl->pItemsTxt[i];

			int dwCode = 0;
			if (pItemsTxtRecord->dwAlternateGfx)
			{
				dwCode = pItemsTxtRecord->dwAlternateGfx;
			}
			else
			{
				dwCode = pItemsTxtRecord->dwCode;
			}

			BOOL bNeedsToBeAdded = FALSE;
			if ((ITEMS_CheckType(pItemsTxtRecord->wType[0], ITEMTYPE_WEAPON)
				|| ITEMS_CheckType(pItemsTxtRecord->wType[0], ITEMTYPE_ARMOR)
				|| ITEMS_CheckType(pItemsTxtRecord->wType[0], ITEMTYPE_ANY_SHIELD)
				|| ITEMS_CheckType(pItemsTxtRecord->wType[0], ITEMTYPE_HELM)) && !ITEMS_CheckType(pItemsTxtRecord->wType[0], ITEMTYPE_CIRCLET))
			{
				bNeedsToBeAdded = TRUE;
			}

			int nCounter = 0;
			while (nCounter < nCurrentTableEntries)
			{
				if (gTxtComponentItemTypeMap[nCounter].dwCode == dwCode)
				{
					break;
				}

				++nCounter;
			}

			if (nCounter >= nCurrentTableEntries && bNeedsToBeAdded == 1)
			{
				int nIndex = nCurrentTableEntries;

				while (ITEMS_CheckType(gComponentItemTypeMap[nIndex].nItemType, ITEMTYPE_WEAPON) && ITEMS_CheckType(pItemsTxtRecord->wType[0], ITEMTYPE_WEAPON)
					   || ITEMS_CheckType(gComponentItemTypeMap[nIndex].nItemType, ITEMTYPE_ANY_ARMOR) && ITEMS_CheckType(pItemsTxtRecord->wType[0], ITEMTYPE_ANY_ARMOR)
					   || gTxtComponentItemTypeMap[nIndex].dwCode)
				{
					++nIndex;
				}

				if (nIndex >= kComponentArrayLogicalSize)
				{
					nIndex = nCurrentTableEntries;
				}

				gTxtComponentItemTypeMap[nIndex].dwCode = dwCode;
				gTxtComponentItemTypeMap[nIndex].nItemType = pItemsTxtRecord->wType[0];

				if (nCurrentTableEntries == nIndex)
				{
					++nCurrentTableEntries;
				}
			}
		}

		gnComponentArrayRecordCount = kComponentArrayLogicalSize;
	}
}

//D2Common.0x6FD917B0
// 1.13c D2Common+0x20E80: crossbow fills the left hand slot from the composite right hand item, bow with the left hand component fills the right hand slot from the "lit " record; 0 from the slot search clears the slot
void __fastcall sub_6FD917B0(D2UnitStrc* pUnit, uint8_t* a2, uint8_t* pColor, D2UnitStrc* pItem)
{
	D2_ASSERT(pUnit);
	D2_ASSERT(pItem);

	const uint8_t nBodyLoc = ITEMS_GetBodyLocation(pItem);
	if (nBodyLoc == BODYLOC_RARM || nBodyLoc == BODYLOC_LARM)
	{
		int nWeaponClass = 0;
		const uint32_t dwWeaponClassCode = COMPOSIT_GetWeaponClassId(pUnit, pUnit->pInventory, &nWeaponClass, -1, 1);
		const uint8_t nComponent = ITEMS_GetComponent(pItem);

		if (dwWeaponClassCode == ' wbx')
		{
			if ((nComponent == COMPOSIT_RIGHTHAND || nComponent == COMPOSIT_LEFTHAND) && pUnit->pInventory)
			{
				D2UnitStrc* pCompositItem = INVENTORY_GetCompositItem(pUnit->pInventory, COMPOSIT_RIGHTHAND);
				if (pCompositItem)
				{
					D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pCompositItem->dwClassId);
					D2_ASSERT(pItemsTxtRecord);

					INVENTORY_InitializeComponentArray();

					const int nIndex = INVENTORY_GetComponentArrayIndexFromItemsTxtRecord(pItemsTxtRecord);

					if (nIndex == 0)
					{
						a2[COMPOSIT_LEFTHAND] = -1;
						pColor[COMPOSIT_LEFTHAND] = -1;
					}
					else
					{
						a2[COMPOSIT_LEFTHAND] = nIndex;
					}
				}
			}
		}
		else if(dwWeaponClassCode == ' wob')
		{
			if (nComponent == COMPOSIT_LEFTHAND)
			{
				int nItemId = 0;
				D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemRecordFromItemCode(' til', &nItemId);
				if (pItemsTxtRecord)
				{
					INVENTORY_InitializeComponentArray();

					const int nIndex = INVENTORY_GetComponentArrayIndexFromItemsTxtRecord(pItemsTxtRecord);

					if (nIndex == 0)
					{
						a2[COMPOSIT_RIGHTHAND] = -1;
						pColor[COMPOSIT_RIGHTHAND] = -1;
					}
					else
					{
						a2[COMPOSIT_RIGHTHAND] = nIndex;
					}
				}
			}
		}
	}
}

//D2Common.0x6FD91B60 (#10299)
//TODO: Find a name
uint32_t D2Common_211D0_BodyLocAnswer(D2UnitStrc* pUn, uint32_t nBodyLoc, D2UnitStrc* pInvItem, int32_t bDontCheckReqs);	// the 1.13c body-location answer (D2Common+0x211D0), proved by the D2Common FN replay
int __stdcall D2Common_10299(D2UnitStrc* pUnit, int nBodyLoc, D2UnitStrc* pItem, BOOL bDontCheckReqs)
{
	return (int)D2Common_211D0_BodyLocAnswer(pUnit, (uint32_t)nBodyLoc, pItem, bDontCheckReqs);
}

//D2Common.0x6FD92080 (#10304)
// 1.13c D2Common+0x1E8F0
D2UnitStrc* __stdcall INVENTORY_GetNextItem(D2UnitStrc* pItem)
{
	D2ItemExtraDataStrc* pItemExtraData = INVENTORY_GetItemExtraDataFromItem(pItem);
	if (pItemExtraData)
	{
		return pItemExtraData->pNextItem;
	}

	return nullptr;
}

// not-1.13c: retail has no function for this; the grid-next link (item data +0x70) is read inline at each caller
D2UnitStrc* __stdcall INVENTORY_GetNextGridItem(D2UnitStrc* pItem)
{
	D2ItemExtraDataStrc* pItemExtraData = INVENTORY_GetItemExtraDataFromItem(pItem);
	if (pItemExtraData)
	{
		return pItemExtraData->pNextGridItem;
	}

	return nullptr;
}

// 1.13c D2Common+0x1E400
//D2Common.0x6FD920C0 (#10305)
D2UnitStrc* __stdcall INVENTORY_UnitIsItem(D2UnitStrc* pItem)
{
	return pItem && pItem->dwUnitType == UNIT_ITEM ? pItem : nullptr;
}

//D2Common.0x6FD92100 (#10307)
// 1.13c D2Common+0x1E8C0
int __stdcall INVENTORY_GetItemNodePage(D2UnitStrc* pItem)
{
	D2ItemExtraDataStrc* pItemExtraData = INVENTORY_GetItemExtraDataFromItem(pItem);
	if (pItemExtraData)
	{
		return pItemExtraData->nNodePosOther;
	}

	return 0;
}

//D2Common.0x6FD92140 (#10310)
// 1.13c D2Common+0x1E880: returns the item when its parent inventory is the given one; no delta
D2UnitStrc* __stdcall INVENTORY_IsItemInInventory(D2InventoryStrc* pInventory, D2UnitStrc* pItem)
{
	if (pInventory)
	{
		D2ItemExtraDataStrc* pItemExtraData = INVENTORY_GetItemExtraDataFromItem(pItem);
		if (pItemExtraData && pInventory == pItemExtraData->pParentInv)
		{
			return pItem;
		}
	}

	return nullptr;
}

//D2Common.0x6FDAFEA0 (#10311)
// 1.13c D2Common+0x1DEC0: null check, next pointer at +0x04
D2InventoryNodeStrc* __stdcall INVENTORY_GetNextNode(D2InventoryNodeStrc* pNode)
{
	if (pNode)
	{
		return pNode->pNext;
	}

	return nullptr;
}

//D2Common.0x6FD90AB0 (#10312)
// 1.13c D2Common+0x1DEB0: null check, dword at +0x00
D2UnitGUID __stdcall INVENTORY_GetItemGUIDFromNode(D2InventoryNodeStrc* pNode)
{
	if (pNode)
	{
		return pNode->nItemId;
	}

	return 0;
}

//D2Common.0x6FD92180 (#10300)
// 1.13c D2Common+0x21710
BOOL __stdcall INVENTORY_RemoveAllItems(D2InventoryStrc* pInventory)
{
	if (!INVENTORY_GetPtrIfValid(pInventory))
	{
		return FALSE;
	}

	pInventory->pCursorItem = nullptr;
	pInventory->dwLeftItemGUID = D2UnitInvalidGUID;
	
	while (pInventory->pFirstItem)
	{
		if (!INVENTORY_RemoveItem(pInventory->pFirstItem))
		{
			return FALSE;
		}
	}

	return TRUE;
}

// 1.13c D2Common+0x20D20: items of the first player's list with item data +0x45 (inventory page) 2 must all fit; a non-item node ends the walk
BOOL __fastcall INVENTORY_CanItemBePlacedInTradeInventory(D2InventoryStrc* pTradeInventory, D2UnitStrc* pPlayer1, D2UnitStrc* pPlayer2)
{
	for (D2UnitStrc* pNode = INVENTORY_GetFirstItem(pPlayer1->pInventory); pNode;)
	{
		D2UnitStrc* pItem = pNode;
		pNode = INVENTORY_GetNextItem(pNode);
		if (pItem->dwUnitType == UNIT_ITEM && pItem->pItemData && pItem->pItemData->nInvPage == INVPAGE_TRADE && !INVENTORY_CanItemBePlacedInInventory(pPlayer2, pItem, pTradeInventory))
		{
			return FALSE;
		}
	}

	return TRUE;
}

// #10762: the 1.13c body at the image layout is D2Common_21C50_CanItemsBeTraded
//D2Common.0x6FD921D0 (#10302)
BOOL __stdcall INVENTORY_CanItemsBeTraded(void* pMemPool, D2UnitStrc* pPlayer1, D2UnitStrc* pPlayer2, D2TradeStates* pTradeState)
{	
	D2InventoryStrc* pTradeInventory1 = INVENTORY_AllocInventory(pMemPool, nullptr);
	D2InventoryStrc* pTradeInventory2 = INVENTORY_AllocInventory(pMemPool, nullptr);

	if (pTradeInventory1 && pTradeInventory2 && INVENTORY_CopyUnitItemsToTradeInventory(pTradeInventory1, pPlayer1) && INVENTORY_CopyUnitItemsToTradeInventory(pTradeInventory2, pPlayer2))
	{
		if (!INVENTORY_CanItemBePlacedInTradeInventory(pTradeInventory2, pPlayer1, pPlayer2))
		{
			if (pTradeState)
			{
				*pTradeState = TRADESTATE_OTHERNOROOM;
			}
		}
		else if (!INVENTORY_CanItemBePlacedInTradeInventory(pTradeInventory1, pPlayer2, pPlayer1))
		{
			if (pTradeState)
			{
				*pTradeState = TRADESTATE_SELFNOROOM;
			}
		}
		else
		{
			INVENTORY_FreeInventory(pTradeInventory1);
			INVENTORY_FreeInventory(pTradeInventory2);
			return TRUE;
		}
	}

	INVENTORY_FreeInventory(pTradeInventory1);
	INVENTORY_FreeInventory(pTradeInventory2);
	return FALSE;
}

//D2Common.0x6FD923C0
// 1.13c D2Common+0x1E7B0
BOOL __fastcall INVENTORY_CopyUnitItemsToTradeInventory(D2InventoryStrc* pTradeInventory, D2UnitStrc* pUnit)
{
	if (INVENTORY_GetPtrIfValid(pTradeInventory))
	{
		const int nInventoryRecordId = UNITS_GetInventoryRecordId(pUnit, INVPAGE_INVENTORY, TRUE);
		D2InventoryGridInfoStrc pInventoryGridInfo = {};
		DATATBLS_GetInventoryGridInfo(nInventoryRecordId, 0, &pInventoryGridInfo);

		D2InventoryGridStrc* pTradeInventoryGrid = INVENTORY_GetGrid(pTradeInventory, INVGRID_INVENTORY, &pInventoryGridInfo);

		if (pTradeInventoryGrid && INVENTORY_GetPtrIfValid(pUnit->pInventory))
		{
			D2InventoryGridStrc* pUnitInventoryGrid = INVENTORY_GetGrid(pUnit->pInventory, INVGRID_INVENTORY, &pInventoryGridInfo);

			if (pUnitInventoryGrid && pUnitInventoryGrid->nGridWidth && pUnitInventoryGrid->nGridHeight
				&& pUnitInventoryGrid->nGridWidth == pTradeInventoryGrid->nGridWidth && pUnitInventoryGrid->nGridHeight == pTradeInventoryGrid->nGridHeight
				&& pUnitInventoryGrid->ppItems && pTradeInventoryGrid->ppItems)
			{
				memcpy(pTradeInventoryGrid->ppItems, pUnitInventoryGrid->ppItems, sizeof(D2UnitStrc*) * pUnitInventoryGrid->nGridWidth * pUnitInventoryGrid->nGridHeight);

				return TRUE;
			}
		}
	}
	
	return FALSE;
}

//D2Common.0x6FD92490
// 1.13c D2Common+0x20A30: validated, no delta (inventory grid of the first record, free position, size and bounds check, then the covered cells marked)
BOOL __fastcall INVENTORY_CanItemBePlacedInInventory(D2UnitStrc* pPlayer, D2UnitStrc* pItem, D2InventoryStrc* pInventory)
{
	if (INVENTORY_GetPtrIfValid(pInventory))
	{
		const int nInventoryRecordId = UNITS_GetInventoryRecordId(pPlayer, INVPAGE_INVENTORY, TRUE);
		D2InventoryGridInfoStrc pInventoryGridInfo = {};
		DATATBLS_GetInventoryGridInfo(nInventoryRecordId, 0, &pInventoryGridInfo);

		D2InventoryGridStrc* pInventoryGrid = INVENTORY_GetGrid(pInventory, INVGRID_INVENTORY, &pInventoryGridInfo);

		int nX = 0;
		int nY = 0;
		if (pInventoryGrid && INVENTORY_GetFreePosition(pInventory, pItem, nInventoryRecordId, &nX, &nY, 0))
		{
			uint8_t nHeight = 0;
			uint8_t nWidth = 0;

			ITEMS_GetDimensions(pItem, &nWidth, &nHeight, __FILE__, __LINE__);

			if (nWidth && nHeight && nX >= 0 && nX + nWidth <= pInventoryGrid->nGridWidth && nY >= 0 && nY + nHeight <= pInventoryGrid->nGridHeight)
			{
				for (int y = nY; y < nY + nHeight; ++y)
				{
					for (int x = nX; x < nX + nWidth; ++x)
					{
						pInventoryGrid->ppItems[x + y * pInventoryGrid->nGridWidth] = (D2UnitStrc*)0xFFFFFFFF;
					}
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}

//D2Common.0x6FD925E0
// not-1.13c: retail reads the unit's path through the path getter (D2Common+0x34BB0) at each call site
int __fastcall UNITS_GetXPosition(D2UnitStrc* pUnit)
{
	switch (pUnit->dwUnitType)
	{
	case UNIT_PLAYER:
	case UNIT_MONSTER:
	case UNIT_MISSILE:
		return pUnit->pDynamicPath ? pUnit->pDynamicPath->tGameCoords.wPosX : 0;

	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		return pUnit->pStaticPath ? pUnit->pStaticPath->tGameCoords.nX : 0;
	default:
		return 0;
	}
}

//D2Common.0x6FD92610
// not-1.13c: retail reads the unit's path through the path getter (D2Common+0x34B80) at each call site
int __fastcall UNITS_GetYPosition(D2UnitStrc* pUnit)
{
	switch (pUnit->dwUnitType)
	{
	case UNIT_PLAYER:
	case UNIT_MONSTER:
	case UNIT_MISSILE:
		return pUnit->pDynamicPath ? pUnit->pDynamicPath->tGameCoords.wPosY : 0;

	case UNIT_OBJECT:
	case UNIT_ITEM:
	case UNIT_TILE:
		return pUnit->pStaticPath ? pUnit->pStaticPath->tGameCoords.nY : 0;
	default:
		return 0;
	}
}

// d2-web: the character-menu appearance bytes (d2s 0x88, written by INVENTORY_GetItemSaveGfxInfo) name a
// component token by its index in gTxtComponentItemTypeMap. Character select draws the portrait from
// them without a player unit: the token (the item's alternate gfx, else its code) and the weapon class
// of the first Items.txt row carrying it. Returns 0 for an index with no token.
extern "C" int D2WEB_ComponentToken(int nIndex, uint32_t* pCode, uint32_t* pWeaponClass)
{
	INVENTORY_InitializeComponentArray();
	if (nIndex <= 0 || nIndex >= kComponentArrayLogicalSize || !gTxtComponentItemTypeMap[nIndex].dwCode) return 0;
	*pCode = (uint32_t)gTxtComponentItemTypeMap[nIndex].dwCode; *pWeaponClass = 0;
	D2ItemDataTbl* pItemDataTbl = DATATBLS_GetItemDataTables();
	for (int i = 0; i < pItemDataTbl->nItemsTxtRecordCount; ++i)
	{
		D2ItemsTxt* p = &pItemDataTbl->pItemsTxt[i];
		if ((uint32_t)(p->dwAlternateGfx ? p->dwAlternateGfx : p->dwCode) == *pCode) { *pWeaponClass = p->dwWeapClass; break; }
	}
	return 1;
}

extern "C" int32_t d2_port_d2common_1f980(D2UnitStrc* unit, D2UnitStrc* first, D2UnitStrc* second);

// 1.13c D2Common+0x20DB0: the hand-pair answer of the body-location placement check (0x211D0's 4/5 and 11/12 cases).
// Retail is a register call: EAX = the item already at the requested location, ECX = the item at the paired location,
// EDX = the item being placed, then the player and the paired location on the stack (ret 8).
//   no incoming item: 3 when the requested location is taken, 4 when the paired item is a two-handed item, else 0;
//   incoming, requested location free: 1 without a paired item, else 2 - (0x1F980 compatible);
//   incoming, requested location taken: 6 when the two stack (0x26930), 5 when 0x1F980 accepts the pair, else 7 when
//     the paired location's item fits the player's inventory page (0x20B50), else 0.
int32_t D2Common_20DB0_HandPairAnswer(D2UnitStrc* pOther, D2UnitStrc* pIncoming, D2UnitStrc* pPlayer,
	int32_t nOtherBodyLoc, D2UnitStrc* pAtLoc)
{
	if (!pIncoming)
	{
		if (pAtLoc)
		{
			return 3;
		}

		if (pOther && pOther->dwUnitType == UNIT_ITEM && ITEMS_CheckWeaponIfTwoHandedByItemId(pOther->dwClassId))
		{
			return 4;
		}

		return 0;
	}

	if (!pAtLoc)
	{
		if (pOther)
		{
			return 2 - (d2_port_d2common_1f980(pPlayer, pIncoming, pOther) != 0);
		}

		return 1;
	}

	if (ITEMS_AreStackablesEqual(pAtLoc, pIncoming))
	{
		return 6;
	}

	if (d2_port_d2common_1f980(pPlayer, pIncoming, pOther))
	{
		return 5;
	}

	// 0x20B50: the paired slot's item, given an incoming item, must have a free position on page 0.
	const int32_t nInventoryRecord = UNITS_GetInventoryRecordId(pPlayer, 0, 1);
	D2InventoryStrc* pInventory = pPlayer->pInventory;
	if (!INVENTORY_GetPtrIfValid(pInventory) || nOtherBodyLoc < 0 || nOtherBodyLoc >= 13)
	{
		return 0;
	}

	D2InventoryGridStrc* pGrid = INVENTORY_GetGrid(pInventory, INVGRID_BODYLOC, &gBodyLocInventoryGridInfo);
	D2UnitStrc* pPaired = pGrid ? pGrid->ppItems[nOtherBodyLoc] : nullptr;
	if (!pPaired)
	{
		return 0;
	}

	int32_t nX = 0;
	int32_t nY = 0;
	return INVENTORY_GetFreePosition(pInventory, pPaired, nInventoryRecord, &nX, &nY, 0) ? 7 : 0;
}
