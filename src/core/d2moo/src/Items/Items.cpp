#include "../../../d2common/varinvgfx_index.h"
#include "D2Items.h"
#include <Units/Item.h>
#include "D2BitManip.h"
#include "D2Composit.h"
#include "D2DataTbls.h"
#include "D2Inventory.h"
#include "D2ItemMods.h"
#include "D2QuestRecord.h"
#include "D2Seed.h"
#include "D2States.h"
#include "D2StatList.h"
#include "Units/Units.h"
#include <D2CMP.h>
#include <D2Math.h>
#include <DataTbls/MonsterIds.h>


//D2Common.0x6FD98380 (#10687)
// 1.13c D2Common+0x23C70
void __stdcall ITEMS_AllocItemData(void* pMemPool, D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		pItem->pItemData = D2_ALLOC_STRC_POOL(pMemPool, D2ItemDataStrc);
		if (!pItem->pItemData)
		{
			//FOG_DisplayHalt("Out of Memory in ITEMSDataInit()", __FILE__, __LINE__);
			exit(-1);
		}
		memset(pItem->pItemData, 0x00, sizeof(D2ItemDataStrc));

		pItem->pItemData->dwOwnerGUID = D2UnitInvalidGUID;
	}
}

// 1.13c D2Common+0x2A5C0
//D2Common.0x6FD983F0 (#10688)
void __stdcall ITEMS_FreeItemData(void* pMemPool, D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		INVENTORY_RemoveItem(pItem);
		if (pItem->pItemData)
		{
			D2_FREE_POOL(pMemPool, pItem->pItemData);
		}
	}
}

//D2Common.0x6FD98430 (#10689)
// 1.13c D2Common+0x23C50
uint8_t __stdcall ITEMS_GetBodyLocation(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->nBodyLoc;
	}

	return BODYLOC_NONE;
}

// 1.13c D2Common+0x23C30
//D2Common.0x6FD98450 (#10690)
void __stdcall ITEMS_SetBodyLocation(D2UnitStrc* pItem, uint8_t nBodyLoc)
{
	if (D2ItemDataStrc* pItemData = ITEMS_GetItemData(pItem))
	{
		pItemData->nBodyLoc = nBodyLoc;
	}
}

// 1.13c D2Common+0x23C10
//D2Common.0x6FD98470 (#10691)
D2SeedStrc* __stdcall ITEMS_GetItemSeed(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc* pItemData = ITEMS_GetItemData(pItem))
	{
		return &pItemData->pSeed;
	}
	
	return NULL;
}

//D2Common.0x6FD98490 (#10692)
// 1.13c D2Common+0x23BE0
void __stdcall ITEMS_InitItemSeed(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc* pItemData = ITEMS_GetItemData(pItem))
	{
		// retail writes the two seed words in place (low 1, high 666)
		pItemData->pSeed.nLowSeed = 1;
		pItemData->pSeed.nHighSeed = 666;
	}
}

// 1.13c D2Common+0x23BC0
//D2Common.0x6FD984B0 (#10693)
int __stdcall ITEMS_GetItemStartSeed(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->dwInitSeed;
	}
	
	return 0;
}

static_assert(offsetof(D2ItemDataStrc, dwInitSeed) == 0x10);
static_assert(offsetof(D2ItemDataStrc, dwFileIndex) == 0x28);

// 1.13c D2Common+0x23B70
// Retail requires a non-null item unit with item data; +0x10 is the initial seed.
//D2Common.0x6FD984D0 (#10694)
void __stdcall ITEMS_SetItemStartSeed(D2UnitStrc* pItem, int nSeed)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);
	D2_ASSERT(pItem->pItemData);
	pItem->pItemData->dwInitSeed = nSeed;
}

// 1.13c D2Common+0x23B40
//D2Common.0x6FD98550 (#10695)
int __stdcall ITEMS_GetItemQuality(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->dwQualityNo;
	}

	return ITEMQUAL_NORMAL;
}

// 1.13c D2Common+0x23B20
//D2Common.0x6FD98580 (#10696)
void __stdcall ITEMS_SetItemQuality(D2UnitStrc* pItem, int nQuality)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		pItemData->dwQualityNo = nQuality;
	}
}

//D2Common.0x6FD985A0 (#10699)
// 1.13c D2Common+0x23AF0
uint16_t __stdcall ITEMS_GetPrefixId(D2UnitStrc* pItem, int nPrefixNo)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->wMagicPrefix[nPrefixNo];
	}

	return 0;
}

// 1.13c D2Common+0x23AC0
//D2Common.0x6FD985D0 (#10700)
void __stdcall ITEMS_AssignPrefix(D2UnitStrc* pItem, uint16_t nPrefix, int nPrefixNo)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		pItemData->wMagicPrefix[nPrefixNo] = nPrefix;
	}
}

//D2Common.0x6FD98600 (#10697)
// 1.13c D2Common+0x23A90 (#11098) returns a dword. Its low word is the affix; the high word is the upper
// half of the item-data pointer in retail's 32-bit space, which has no 64-bit counterpart, so it stays 0.
uint32_t __stdcall ITEMS_GetAutoAffix(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->wAutoAffix; // d2-web: dword return, high word 0
	}

	return 0;
}

// 1.13c D2Common+0x23A70
//D2Common.0x6FD98630 (#10698)
void __stdcall ITEMS_SetAutoAffix(D2UnitStrc* pItem, uint16_t nAffix)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		pItemData->wAutoAffix = nAffix;
	}
}

//D2Common.0x6FD98650 (#10701)
// 1.13c D2Common+0x23A40
uint16_t __stdcall ITEMS_GetSuffixId(D2UnitStrc* pItem, int nSuffixNo)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->wMagicSuffix[nSuffixNo];
	}

	return 0;
}

// 1.13c D2Common+0x23A10
//D2Common.0x6FD98680 (#10702)
void __stdcall ITEMS_AssignSuffix(D2UnitStrc* pItem, uint16_t nSuffix, int nSuffixNo)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		pItemData->wMagicSuffix[nSuffixNo] = nSuffix;
	}
}

// 1.13c D2Common+0x239E0
//D2Common.0x6FD986B0 (#10703)
uint16_t __stdcall ITEMS_GetRarePrefixId(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->wRarePrefix;
	}

	return 0;
}

// 1.13c D2Common+0x239C0
//D2Common.0x6FD986E0 (#10704)
void __stdcall ITEMS_AssignRarePrefix(D2UnitStrc* pItem, uint16_t nPrefix)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		pItemData->wRarePrefix = nPrefix;
	}
}

// 1.13c D2Common+0x23990
//D2Common.0x6FD98700 (#10705)
uint16_t __stdcall ITEMS_GetRareSuffixId(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->wRareSuffix;
	}

	return 0;
}

// 1.13c D2Common+0x23970
//D2Common.0x6FD98730 (#10706)
void __stdcall ITEMS_AssignRareSuffix(D2UnitStrc* pItem, uint16_t nSuffix)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		pItemData->wRareSuffix = nSuffix;
	}
}

//D2Common.0x6FD98750 (#10707)
// 1.13c D2Common+0x23940 (item data +0x18, the item flags; 0x238B0 is the command-flag twin): retail takes only (pItem, dwFlag); nLine and szFile are the 1.10f debug
// arguments, kept so existing callers compile, and are never read.
BOOL __stdcall ITEMS_CheckItemFlag(D2UnitStrc* pItem, uint32_t dwFlag, int nLine, const char* szFile)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->dwItemFlags & dwFlag;
	}

	return 0;
}

//D2Common.0x6FD98780 (#10708)
// 1.13c D2Common+0x23900
void __stdcall ITEMS_SetItemFlag(D2UnitStrc* pItem, uint32_t dwFlag, BOOL bSet)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		if (bSet)
		{
			pItemData->dwItemFlags |= dwFlag;
		}
		else
		{
			pItemData->dwItemFlags &= ~dwFlag;
		}
	}
}

// 1.13c D2Common+0x238E0
//D2Common.0x6FD987C0 (#10709)
uint32_t __stdcall ITEMS_GetItemFlags(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->dwItemFlags;
	}

	return 0;
}

//D2Common.0x6FD987E0 (#10710)
// 1.13c D2Common+0x238B0
BOOL __stdcall ITEMS_CheckItemCMDFlag(D2UnitStrc* pItem, int nFlag)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->dwCommandFlags & nFlag;
	}

	return 0;
}

//D2Common.0x6FD98810 (#10711)
// 1.13c D2Common+0x23870
void __stdcall ITEMS_SetItemCMDFlag(D2UnitStrc* pItem, int nFlag, BOOL bSet)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		if (bSet)
		{
			pItemData->dwCommandFlags |= nFlag;
		}
		else
		{
			pItemData->dwCommandFlags &= ~nFlag;
		}
	}
}

//D2Common.0x6FD98850 (#10712)
// 1.13c D2Common+0x23850
uint32_t __stdcall ITEMS_GetItemCMDFlags(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->dwCommandFlags;
	}

	return 0;
}

//D2Common.0x6FD98870 (#10717)
// 1.13c D2Common+0x23810
int __stdcall ITEMS_GetItemLevel(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		if (static_cast<int32_t>(pItemData->dwItemLevel) < 1)
		{
			pItemData->dwItemLevel = 1;
		}
		return pItemData->dwItemLevel;
	}

	return 1;
}

// 1.13c D2Common+0x237E0
//D2Common.0x6FD988B0 (#10718)
void __stdcall ITEMS_SetItemLevel(D2UnitStrc* pItem, int nItemLevel)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		if (nItemLevel < 1)
		{
			nItemLevel = 1;
		}
		pItemData->dwItemLevel = nItemLevel;
	}
}

//D2Common.0x6FD988E0 (#10719)
// 1.13c D2Common+0x237C0
uint8_t __stdcall ITEMS_GetInvPage(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->nInvPage;
	}

	return -1;
}

// 1.13c D2Common+0x237A0
//D2Common.0x6FD98900 (#10720)
void __stdcall ITEMS_SetInvPage(D2UnitStrc* pItem, uint8_t nPage)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		pItemData->nInvPage = nPage;
	}
}

//D2Common.0x6FD98920 (#10721)
// 1.13c D2Common+0x23780
uint8_t __stdcall ITEMS_GetCellOverlap(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->nCellOverlap;
	}

	return -1;
}

//D2Common.0x6FD98940 (#10722)
// 1.13c D2Common+0x23760
void __stdcall ITEMS_SetCellOverlap(D2UnitStrc* pItem, int nCellOverlap)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		pItemData->nCellOverlap = nCellOverlap;
	}
}

//D2Common.0x6FD98960 (#10853)
// 1.13c D2Common+0x23740
uint8_t __stdcall ITEMS_GetItemCell(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->nItemCell;
	}

	return -1;
}

//D2Common.0x6FD98980 (#10854)
// 1.13c D2Common+0x23720
void __stdcall ITEMS_SetItemCell(D2UnitStrc* pItem, int nItemCell)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		pItemData->nItemCell = nItemCell;
	}
}

//D2Common.0x6FD989A0 (#10723)
// 1.13c D2Common+0x23700
char* __stdcall ITEMS_GetEarName(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->szPlayerName;
	}

	return NULL;
}

//D2Common.0x6FD989C0 (#10724)
// 1.13c D2Common+0x236D0
void __stdcall ITEMS_SetEarName(D2UnitStrc* pItem, char* szName)
{
	int nCounter = 0;

	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		do
		{
			pItemData->szPlayerName[nCounter] = szName[nCounter];
			++nCounter;
		}
		while (szName[nCounter -1]);
	}
}

//D2Common.0x6FD989F0 (#10725)
// 1.13c D2Common+0x236B0
uint8_t __stdcall ITEMS_GetEarLevel(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->nEarLvl;
	}

	return 1;
}

//D2Common.0x6FD98A10 (#10726)
// 1.13c D2Common+0x23690
void __stdcall ITEMS_SetEarLevel(D2UnitStrc* pItem, uint8_t nLevel)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		pItemData->nEarLvl = nLevel;
	}
}

//D2Common.0x6FD98A30 (#10727)
// 1.13c D2Common+0x23670
uint8_t __stdcall ITEMS_GetVarGfxIndex(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->nInvGfxIdx;
	}

	return 0;
}

//D2Common.0x6FD98A50 (#10728)
// 1.13c D2Common+0x23650
void __stdcall ITEMS_SetVarGfxIndex(D2UnitStrc* pItem, uint8_t nIndex)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		pItemData->nInvGfxIdx = nIndex;
	}
}

// 1.13c D2Common+0x24F00 (no ordinal; the item arrives in ESI): a repairable item type whose
// item is a throwable, stackable, non-ethereal stack -- such stacks are "repaired" by refilling.
BOOL ITEMS_IsRepairableStack(D2UnitStrc* pItem)   // not static: the 0x24F00 proof probe calls it
{
	D2_ASSERT(pItem);

	int nItemType = 0;
	if (pItem->dwUnitType == UNIT_ITEM)
	{
		nItemType = ITEMS_GetItemTypeFromItemId(pItem->dwClassId);
		if (nItemType < 0)
		{
			return FALSE;
		}
	}

	if (nItemType < sgptDataTables->nItemTypesTxtRecordCount)
	{
		D2ItemTypesTxt* pItemTypesTxtRecord = &sgptDataTables->pItemTypesTxt[nItemType];
		if (pItemTypesTxtRecord->nRepair && ITEMS_CheckIfThrowable(pItem) && ITEMS_CheckIfStackable(pItem) && !ITEMS_IsEthereal(pItem))
		{
			return TRUE;
		}
	}

	return FALSE;
}

//D2Common.0x6FD98A70 (#10777)
// 1.13c D2Common+0x27090: an identified, non-ethereal item with a charged skill below its maximum
// is repairable outright; otherwise its type's repair flag decides, and a refillable stack or a
// breakable durability item qualifies.
BOOL __stdcall ITEMS_IsRepairable(D2UnitStrc* pItem)
{
	D2ItemDataStrc* pItemData = ITEMS_GetItemData(pItem);
	if (!pItemData || !(pItemData->dwItemFlags & IFLAG_IDENTIFIED) || (pItemData->dwItemFlags & IFLAG_ETHEREAL))
	{
		return FALSE;
	}

	BOOL bChargesFull = FALSE;
	if (!ITEMS_HasUsedCharges(pItem, &bChargesFull) || bChargesFull)
	{
		D2ItemTypesTxt* pItemTypesTxtRecord = DATATBLS_GetItemTypesTxtRecord(ITEMS_GetItemType(pItem));
		if (!pItemTypesTxtRecord || !pItemTypesTxtRecord->nRepair)
		{
			return FALSE;
		}

		if (!ITEMS_IsRepairableStack(pItem))
		{
			return ITEMS_HasDurability(pItem);
		}
	}

	return TRUE;
}

//D2Common.0x6FD98C60 (#10780)
// 1.13c D2Common+0x23610
int32_t __stdcall ITEMS_GetAmmoTypeFromItemType(int nItemType)
{
	if (nItemType >= 0 && nItemType < sgptDataTables->nItemTypesTxtRecordCount)
	{
		return (int16_t)sgptDataTables->pItemTypesTxt[nItemType].wShoots;
	}

	return 0;
}

//D2Common.0x6FD98CA0 (#10781)
// 1.13c D2Common+0x24EB0
int32_t __stdcall ITEMS_GetAmmoType(D2UnitStrc* pItem)
{
	// 1.13c: item type via ITEMS_GetItemType (0x230F0), table row sign-extended
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		return ITEMS_GetAmmoTypeFromItemType((int)ITEMS_GetItemType(pItem));
	}

	return 0;
}

//D2Common.0x6FD98D20 (#10782)
// 1.13c D2Common+0x235D0
int32_t __stdcall ITEMS_GetQuiverTypeFromItemType(int nItemType)
{
	if (nItemType >= 0 && nItemType < sgptDataTables->nItemTypesTxtRecordCount)
	{
		return (int16_t)sgptDataTables->pItemTypesTxt[nItemType].wQuiver;
	}

	return 0;
}

//D2Common.0x6FD98D60 (#10783)
// 1.13c D2Common+0x24E60
int32_t __stdcall ITEMS_GetQuiverType(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		return ITEMS_GetQuiverTypeFromItemType((int)ITEMS_GetItemType(pItem));
	}

	return 0;
}

//D2Common.0x6FD98DE0 (#10784)
// 1.13c D2Common+0x23590
uint32_t __stdcall ITEMS_GetAutoStackFromItemType(int nItemType)
{
	D2ItemTypesTxt* pItemTypesTxtRecord = NULL;

	if (nItemType >= 0 && nItemType < sgptDataTables->nItemTypesTxtRecordCount)
	{
		pItemTypesTxtRecord = &sgptDataTables->pItemTypesTxt[nItemType];
		{
			return pItemTypesTxtRecord->nAutoStack;
		}
	}

	return 0;
}

//D2Common.0x6FD98E20 (#10785)
// 1.13c D2Common+0x24E10: the item type comes from ITEMS_GetItemType, then a bounds-checked
// ItemTypes.txt row.
uint32_t __stdcall ITEMS_GetAutoStack(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		const int nItemType = ITEMS_GetItemType(pItem);
		if (nItemType >= 0 && nItemType < sgptDataTables->nItemTypesTxtRecordCount)
		{
			return sgptDataTables->pItemTypesTxt[nItemType].nAutoStack;
		}
	}

	return 0;
}

// 1.13c D2Common+0x23540
//D2Common.0x6FD98EA0 (#10786)
uint32_t __stdcall ITEMS_GetReload(D2UnitStrc* pItem)
{
	D2ItemTypesTxt* pItemTypesTxtRecord = NULL;
	D2ItemsTxt* pItemsTxtRecord = NULL;

	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		D2_ASSERT(pItemsTxtRecord);

		if (pItemsTxtRecord->wType[0] >= 0 && pItemsTxtRecord->wType[0] < sgptDataTables->nItemTypesTxtRecordCount)
		{
			pItemTypesTxtRecord = &sgptDataTables->pItemTypesTxt[pItemsTxtRecord->wType[0]];
			if (pItemTypesTxtRecord)
			{
				return pItemTypesTxtRecord->nReload;
			}
		}
	}

	return 0;
}

//D2Common.0x6FD98F20 (#10787)
// 1.13c D2Common+0x234F0
uint32_t __stdcall ITEMS_GetReEquip(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		const int nItemType = ITEMS_GetItemType(pItem);
		if (nItemType >= 0 && nItemType < sgptDataTables->nItemTypesTxtRecordCount)
		{
			return sgptDataTables->pItemTypesTxt[nItemType].nReEquip;
		}
	}

	return 0;
}

//D2Common.0x6FD98FA0 (#10788)
// 1.13c D2Common+0x234A0
uint8_t __stdcall ITEMS_GetStorePage(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		const int nItemType = ITEMS_GetItemType(pItem);
		if (nItemType >= 0 && nItemType < sgptDataTables->nItemTypesTxtRecordCount)
		{
			const uint8_t nStorePage = sgptDataTables->pItemTypesTxt[nItemType].nStorePage;
			if (nStorePage != (uint8_t)-1)
			{
				return nStorePage;
			}
		}
	}

	return -1;
}

//D2Common.0x6FD99020 (#10789)
// 1.13c D2Common+0x23450
uint8_t __stdcall ITEMS_GetVarInvGfxCount(D2UnitStrc* pItem)
{
	// 1.13c: item type via ITEMS_GetItemType (0x230F0)
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		const int32_t nType = (int32_t)ITEMS_GetItemType(pItem);
		if (nType >= 0 && nType < sgptDataTables->nItemTypesTxtRecordCount)
		{
			return sgptDataTables->pItemTypesTxt[nType].nVarInvGfx;
		}
	}

	return 0;
}

//D2Common.0x6FD990A0 (#10790)
// 1.13c D2Common+0x24DA0
char* __stdcall ITEMS_GetVarInvGfxString(D2UnitStrc* pItem, int nId)
{
	// 1.13c: type via ITEMS_GetItemType (0x230F0), no assert, signed-min clamp (see varinvgfx_index.h)
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		const int32_t nType = (int32_t)ITEMS_GetItemType(pItem);
		if (nType >= 0 && nType < sgptDataTables->nItemTypesTxtRecordCount)
		{
			D2ItemTypesTxt* pType = &sgptDataTables->pItemTypesTxt[nType];
			return pType->szInvGfx[d2common_varinvgfx_index(nId, pType->nVarInvGfx)];
		}
	}

	return &sgptDataTables->szDefaultString;
}

//D2Common.0x6FD99140 (#10792)
// 1.13c D2Common+0x23400
BOOL __stdcall ITEMS_CanBeRare(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		const int32_t nType = (int32_t)ITEMS_GetItemType(pItem);
		if (nType >= 0 && nType < sgptDataTables->nItemTypesTxtRecordCount)
		{
			return sgptDataTables->pItemTypesTxt[nType].nRare;
		}
	}

	return 0;
}

//D2Common.0x6FD991C0 (#10791)
// 1.13c D2Common+0x233B0
BOOL __stdcall ITEMS_CanBeMagic(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		const int32_t nType = (int32_t)ITEMS_GetItemType(pItem);
		if (nType >= 0 && nType < sgptDataTables->nItemTypesTxtRecordCount)
		{
			return sgptDataTables->pItemTypesTxt[nType].nMagic;
		}
	}

	return 0;
}

//D2Common.0x6FD99240 (#10793)
// 1.13c D2Common+0x23360
BOOL __stdcall ITEMS_CanBeNormal(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		const int32_t nType = (int32_t)ITEMS_GetItemType(pItem);
		if (nType >= 0 && nType < sgptDataTables->nItemTypesTxtRecordCount)
		{
			return sgptDataTables->pItemTypesTxt[nType].nNormal;
		}
	}

	// 1.13c: a non-item or an unknown item type can be normal
	return 1;
}

// 1.13c D2Common+0x23330
//D2Common.0x6FD992C0 (#10744)
uint32_t __stdcall ITEMS_GetWeaponClassCode(D2UnitStrc* pItem)
{
	D2ItemsTxt* pItemsTxtRecord = NULL;

	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		if (pItemsTxtRecord)
		{
			return pItemsTxtRecord->dwWeapClass;
		}
	}

	return 0;
}

//D2Common.0x6FD992F0 (#10745)
// 1.13c D2Common+0x232E0
uint32_t __stdcall ITEMS_Get2HandWeaponClassCode(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);
	return pItemsTxtRecord->dwWeapClass2Hand;
}

//D2Common.0x6FD99370 (#10746)
// 1.13c D2Common+0x23290 (1.13c reads items.txt +0x80 = dwCode; asserts null/non-item/no-record)
static_assert(offsetof(D2ItemsTxt, dwCode) == 0x80, "1.13c sub_23290 reads +0x80");
uint32_t __stdcall ITEMS_GetBaseCode(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);
	return pItemsTxtRecord->dwCode;
}

//D2Common.0x6FD993F0 (#10747)
// 1.13c D2Common+0x23230
uint32_t __stdcall ITEMS_GetAltGfx(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);
	if (pItemsTxtRecord->dwAlternateGfx)
	{
		return pItemsTxtRecord->dwAlternateGfx;
	}
	else
	{
		return pItemsTxtRecord->dwCode;
	}
}

//D2Common.0x6FD99480 (#10748)
// 1.13c D2Common+0x231E0
uint8_t __stdcall ITEMS_GetComponent(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);

	return pItemsTxtRecord->nComponent;
}

//D2Common.0x6FD99500 (#10749)
// 1.13c D2Common+0x231A0
void __stdcall ITEMS_GetDimensions(D2UnitStrc* pItem, uint8_t* pWidth, uint8_t* pHeight, const char* szFile, int nLine)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		if (D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId))
		{
			*pWidth = pItemsTxtRecord->nInvWidth;
			*pHeight = pItemsTxtRecord->nInvHeight;
		}
	}
}

//D2Common.0x6FD99540 (#10750)
// 1.13c D2Common+0x23130
void __stdcall ITEMS_GetAllowedBodyLocations(D2UnitStrc* pItem, uint8_t* pBodyLoc1, uint8_t* pBodyLoc2)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		const int nItemType = ITEMS_GetItemType(pItem);
		if (nItemType >= 0 && nItemType < sgptDataTables->nItemTypesTxtRecordCount)
		{
			D2ItemTypesTxt* pItemTypesTxtRecord = &sgptDataTables->pItemTypesTxt[nItemType];
			*pBodyLoc1 = pItemTypesTxtRecord->nBodyLoc1;
			*pBodyLoc2 = pItemTypesTxtRecord->nBodyLoc2;
			return;
		}
	}

	*pBodyLoc1 = 0;
	*pBodyLoc2 = 0;
}

//D2Common.0x6FD995D0 (#10751)
// 1.13c D2Common+0x230F0
uint32_t __stdcall ITEMS_GetItemType(D2UnitStrc* pItem)
{
	D2ItemsTxt* pItemsTxtRecord = NULL;

	D2_ASSERT(pItem);

	if (pItem->dwUnitType == UNIT_ITEM)
	{
		pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		D2_ASSERT(pItemsTxtRecord);
		return pItemsTxtRecord->wType[0];
	}

	return 0;
}

//D2Common.0x6FD99640 (#10752)
// 1.13c D2Common+0x230A0
uint32_t __stdcall ITEMS_GetItemTypeFromItemId(uint32_t dwItemId)
{
	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(dwItemId);

	D2_ASSERT(pItemsTxtRecord);

	return pItemsTxtRecord->wType[0];
}

//D2Common.0x6FD99680 (#10753)
// 1.13c D2Common+0x23050
uint8_t __stdcall ITEMS_GetItemQlvl(const D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);
	
	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);
	return pItemsTxtRecord->nLevel;
}

//D2Common.0x6FD99700 (#10754)
// 1.13c D2Common+0x21FE0
int __stdcall ITEMS_CheckIfFlagIsSet(int nFlags, int nFlag)
{
	return nFlags & nFlag;
}

//D2Common.0x6FD99710 (#10755)
// 1.13c D2Common+0x21FB0
void __stdcall ITEMS_SetOrRemoveFlag(int* pFlags, int nFlag, BOOL bSet)
{
	if (bSet)
	{
		*pFlags |= nFlag;
	}
	else
	{
		*pFlags &= ~nFlag;
	}
}

// 1.13c D2Common+0x266E0: sum of a stat over the items socketed in pItem; only counted while pItem itself
// is equipped (the walk still runs, as in retail, for any other mode and adds nothing).
static int ITEMS_GetBonusStatFromSockets(D2UnitStrc* pItem, D2C_ItemStats stat)
{
	int nStatBonusFromSockets = 0;
	if (pItem && pItem->dwUnitType == UNIT_ITEM && ITEMS_CheckIfSocketableByItemId(pItem->dwClassId))
	{
		for (D2UnitStrc* i = INVENTORY_GetFirstItem(pItem->pInventory); i != NULL; )
		{
			if (pItem->dwAnimMode == IMODE_EQUIP)
			{
				nStatBonusFromSockets += STATLIST_UnitGetStatValue(i->dwUnitType == UNIT_ITEM ? i : NULL, stat, 0);
			}

			if (i->dwUnitType != UNIT_ITEM || !i->pItemData)
			{
				break;
			}

			i = INVENTORY_GetNextItem(i);
		}
	}
	return nStatBonusFromSockets;
}

//D2Common.0x6FD99740 (#10756)
// 1.13c D2Common+0x26DB0: retail has no separate stat-requirement helper; the strength and dexterity
// tests are written out here, strength alone with the owner test.
BOOL __stdcall ITEMS_CheckRequirements(D2UnitStrc* pItem, D2UnitStrc* pUnit, BOOL bEquipping, BOOL* bStrength, BOOL* bDexterity, BOOL* bLevel)
{
	if (bStrength)
	{
		*bStrength = FALSE;
	}

	if (bDexterity)
	{
		*bDexterity = FALSE;
	}

	if (bLevel)
	{
		*bLevel = FALSE;
	}

	if (pItem == NULL || pItem->dwUnitType != UNIT_ITEM)
	{
		return FALSE;
	}


	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	if (!pItemsTxtRecord)
	{
		return FALSE;
	}

	BOOL bStrReqMet = TRUE;
	BOOL bDexReqMet = TRUE;
	BOOL bLevelReqMet = TRUE;

	const D2ItemDataStrc* pItemData = pItem->pItemData;
	const uint32_t nIdentified = pItemData ? (pItemData->dwItemFlags & IFLAG_IDENTIFIED) : 0;
	const int nBaseReqStr = pItemsTxtRecord->wReqStr;
	const int nBaseReqDex = pItemsTxtRecord->wReqDex;

	int nReqStrBonus = 0;
	int nReqDexBonus = 0;
	D2ItemStatCostTxt* pReqPercentTxt = ITEMS_GetItemStatCostTxtRecord(STAT_ITEM_REQ_PERCENT);
	if (pItem->pStatListEx && pReqPercentTxt)
	{
		const int nReqPctBonus = STATLIST_GetTotalStat_6FDB63E0(pItem->pStatListEx, D2SLayerStatIdStrc::MakeFromStatId(STAT_ITEM_REQ_PERCENT).nPackedValue, pReqPercentTxt);
		if (nReqPctBonus)
		{
			nReqStrBonus = DATATBLS_ApplyRatio(nBaseReqStr, nReqPctBonus, 100);
			nReqDexBonus = DATATBLS_ApplyRatio(nBaseReqDex, nReqPctBonus, 100);
		}
	}

	if (pItemData && (pItemData->dwItemFlags & IFLAG_ETHEREAL))
	{
		nReqStrBonus -= 10;
		nReqDexBonus -= 10;
	}

	const int nStrength = STATLIST_UnitGetStatValue(pUnit, STAT_STRENGTH, 0);
	if (nStrength < 1)
	{
		bStrReqMet = FALSE;
	}
	else
	{
		const int nReqStr = nBaseReqStr + nReqStrBonus;
		if (nStrength < nReqStr)
		{
			bStrReqMet = FALSE;
		}
		else if (bEquipping && STATLIST_GetOwner(pItem, NULL))
		{
			const int nSocketBonus = ITEMS_GetBonusStatFromSockets(pItem, STAT_STRENGTH);
			if (nStrength - nSocketBonus < 1 || nStrength - nSocketBonus < nReqStr)
			{
				bStrReqMet = FALSE;
			}
		}
	}

	// Retail does not test the owner for dexterity.
	const int nDexterity = STATLIST_UnitGetStatValue(pUnit, STAT_DEXTERITY, 0);
	if (nDexterity < 1)
	{
		bDexReqMet = FALSE;
	}
	else
	{
		const int nReqDex = nBaseReqDex + nReqDexBonus;
		if (nDexterity < nReqDex)
		{
			bDexReqMet = FALSE;
		}
		else if (bEquipping)
		{
			const int nSocketBonus = ITEMS_GetBonusStatFromSockets(pItem, STAT_DEXTERITY);
			bDexReqMet = nDexterity - nSocketBonus > 0;
			if (nDexterity - nSocketBonus < nReqDex)
			{
				bDexReqMet = FALSE;
			}
		}
	}

	const int nLevelReq = ITEMS_GetRequiredLevel(pItem, pUnit);
	if (nLevelReq != -1 && STATLIST_UnitGetStatValue(pUnit, STAT_LEVEL, 0) < nLevelReq)
	{
		bLevelReqMet = FALSE;
	}

	if (bStrength)
	{
		*bStrength = bStrReqMet;
	}

	if (bDexterity)
	{
		*bDexterity = bDexReqMet;
	}

	if (bLevel)
	{
		*bLevel = bLevelReqMet;
	}

	if (bStrReqMet && bDexReqMet && bLevelReqMet && nIdentified)
	{
		if (ITEMS_GetItemType(pItem) == ITEMTYPE_BOOK && STATLIST_UnitGetStatValue(pItem, STAT_QUANTITY, 0) <= 0)
		{
			return FALSE;
		}

		const uint8_t nClassReq = (uint8_t)ITEMS_GetClassOfClassSpecificItem(pItem);
		if (nClassReq == NUMBER_OF_PLAYERCLASSES)
		{
			return TRUE;
		}

		if (pUnit)
		{
			// 1.13c D2Common+0x1280: the monster id passes only while it is a valid monstats row
			if (pUnit->dwUnitType == UNIT_MONSTER && nClassReq == PCLASS_BARBARIAN
				&& (int32_t)pUnit->dwClassId >= 0 && (int32_t)pUnit->dwClassId < sgptDataTables->nMonStatsTxtRecordCount
				&& pUnit->dwClassId >= MONSTER_ACT5HIRE1 && pUnit->dwClassId <= MONSTER_ACT5HIRE2)
			{
				return TRUE;
			}

			if (pUnit->dwUnitType == UNIT_PLAYER && nClassReq == pUnit->dwClassId)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

//D2Common.0x6FD99BC0 (#10741)
// 1.13c D2Common+0x24D00: an armor or weapon that is its own exceptional or elite version, is not
// of the missile-potion type (by ITEMS_GetItemTypeFromItemId) and is not a quest item.
BOOL __stdcall ITEMS_GetQuestFromItemId(int nItemId)
{
	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);

	if (!ITEMS_CheckItemTypeIdByItemId(nItemId, ITEMTYPE_ANY_ARMOR) && !ITEMS_CheckItemTypeIdByItemId(nItemId, ITEMTYPE_WEAPON))
	{
		return FALSE;
	}

	if ((pItemsTxtRecord->dwUberCode == pItemsTxtRecord->dwCode || pItemsTxtRecord->dwUltraCode == pItemsTxtRecord->dwCode)
		&& ITEMS_GetItemTypeFromItemId(nItemId) != ITEMTYPE_MISSILE_POTION)
	{
		return pItemsTxtRecord->nQuest == 0;
	}

	return FALSE;
}

//D2Common.0x6FD99C60 (#10742)
// 1.13c D2Common+0x25BD0
BOOL __stdcall ITEMS_GetQuest(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	return ITEMS_GetQuestFromItemId(pItem->dwClassId);
}

// Retail fatal: Ordinal_10024(&DAT_6fdda728, Ordinal_10265(code)); _exit(-1). Seams are weak, as in d2common_batch_1a330.
__attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
__attribute__((weak)) void Ordinal_10024(const void*, uint32_t);
__attribute__((weak)) char DAT_6fdda728[1];
[[noreturn]] static void ITEMS_FatalCode(uint32_t code)
{
	if (Ordinal_10265 && Ordinal_10024)
		Ordinal_10024(DAT_6fdda728, Ordinal_10265(code));
	__builtin_trap();
}

//D2Common.0x6FD99D40 (#10743)
// 1.13c D2Common+0x22FF0
uint32_t __stdcall ITEMS_GetNormalCode(D2UnitStrc* pItem)
{
	if (pItem == nullptr)
		ITEMS_FatalCode(0x798);
	if (pItem->dwUnitType != UNIT_ITEM)
		ITEMS_FatalCode(0x799);

	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	if (pItemsTxtRecord->dwNormCode)
	{
		return pItemsTxtRecord->dwNormCode;
	}
	else
	{
		return pItemsTxtRecord->dwCode;
	}
}

//D2Common.0x6FD99DB0
// 1.13c D2Common+0x26190
int __fastcall ITEMS_GetRequiredLevel(D2UnitStrc* pItem, D2UnitStrc* pPlayer)
{
	D2UniqueItemsTxt* pUniqueItemsTxtRecord = NULL;
	D2MagicAffixTxt* pAutoAffixTxtRecord = NULL;
	D2MagicAffixTxt* pPrefixTxtRecord = NULL;
	D2MagicAffixTxt* pSuffixTxtRecord = NULL;
	D2SetItemsTxt* pSetItemsTxtRecord = NULL;
	D2SkillsTxt* pSkillsTxtRecord = NULL;
	D2ItemsTxt* pItemsTxtRecord = NULL;
	int nRequiredLevelForSocketables = 0;
	int nRequiredLevelForSkill = 0;
	int nLevelRequirement = 0;
	int nRequiredLevel = 0;
	int nCraftBonus = 0;
	D2StatStrc pStat[64] = {};
	
	// retail: a missing item data falls back to quality 2 (the default arm) and still reads items.txt, the sockets and the stats
	D2ItemDataStrc* pItemData = ITEMS_GetItemData(pItem);

	nRequiredLevel = 0;
	switch (pItemData ? pItemData->dwQualityNo : ITEMQUAL_NORMAL)
	{
	case ITEMQUAL_MAGIC:
		pPrefixTxtRecord = DATATBLS_GetMagicAffixTxtRecord(pItemData->wMagicPrefix[0]);
		pSuffixTxtRecord = DATATBLS_GetMagicAffixTxtRecord(pItemData->wMagicSuffix[0]);
		pAutoAffixTxtRecord = DATATBLS_GetMagicAffixTxtRecord(pItemData->wAutoAffix);

		if (pPrefixTxtRecord)
		{
			nLevelRequirement = pPrefixTxtRecord->nClass != uint8_t(-1) && pPlayer && pPrefixTxtRecord->nClass == pPlayer->dwClassId ? pPrefixTxtRecord->nClassLevelReq : pPrefixTxtRecord->nLevelReq;
			if (nLevelRequirement >= 0)
			{
				nRequiredLevel = nLevelRequirement;
			}
		}

		if (pSuffixTxtRecord)
		{
			nLevelRequirement = pSuffixTxtRecord->nClass != uint8_t(-1) && pPlayer && pSuffixTxtRecord->nClass == pPlayer->dwClassId ? pSuffixTxtRecord->nClassLevelReq : pSuffixTxtRecord->nLevelReq;
			if (nRequiredLevel <= nLevelRequirement)
			{
				nRequiredLevel = nLevelRequirement;
			}
		}

		if (pAutoAffixTxtRecord)
		{
			nLevelRequirement = pAutoAffixTxtRecord->nClass != uint8_t(-1) && pPlayer && pAutoAffixTxtRecord->nClass == pPlayer->dwClassId ? pAutoAffixTxtRecord->nClassLevelReq : pAutoAffixTxtRecord->nLevelReq;
			if (nRequiredLevel <= nLevelRequirement)
			{
				nRequiredLevel = nLevelRequirement;
			}
		}
		break;
	case ITEMQUAL_RARE:
		for(int i = 0; i < ITEMS_MAX_MODS; ++i)
		{
			pPrefixTxtRecord = DATATBLS_GetMagicAffixTxtRecord(pItemData->wMagicPrefix[i]);
			pSuffixTxtRecord = DATATBLS_GetMagicAffixTxtRecord(pItemData->wMagicSuffix[i]);

			if (pPrefixTxtRecord)
			{
				nLevelRequirement = pPrefixTxtRecord->nClass != uint8_t(-1) && pPlayer && pPrefixTxtRecord->nClass == pPlayer->dwClassId ? pPrefixTxtRecord->nClassLevelReq : pPrefixTxtRecord->nLevelReq;
				if (nRequiredLevel <= nLevelRequirement)
				{
					nRequiredLevel = nLevelRequirement;
				}
			}

			if (pSuffixTxtRecord)
			{
				nLevelRequirement = pSuffixTxtRecord->nClass != uint8_t(-1) && pPlayer && pSuffixTxtRecord->nClass == pPlayer->dwClassId ? pSuffixTxtRecord->nClassLevelReq : pSuffixTxtRecord->nLevelReq;
				if (nRequiredLevel <= nLevelRequirement)
				{
					nRequiredLevel = nLevelRequirement;
				}
			}
		}

		pAutoAffixTxtRecord = DATATBLS_GetMagicAffixTxtRecord(pItemData->wAutoAffix);
		if (pAutoAffixTxtRecord)
		{
			nLevelRequirement = pAutoAffixTxtRecord->nClass != uint8_t(-1) && pPlayer && pAutoAffixTxtRecord->nClass == pPlayer->dwClassId ? pAutoAffixTxtRecord->nClassLevelReq : pAutoAffixTxtRecord->nLevelReq;
			if (nRequiredLevel <= nLevelRequirement)
			{
				nRequiredLevel = nLevelRequirement;
			}
		}
		break;
	case ITEMQUAL_CRAFT:
		nCraftBonus = 10;
		for (int i = 0; i < ITEMS_MAX_MODS; ++i)
		{
			pPrefixTxtRecord = DATATBLS_GetMagicAffixTxtRecord(pItemData->wMagicPrefix[i]);
			pSuffixTxtRecord = DATATBLS_GetMagicAffixTxtRecord(pItemData->wMagicSuffix[i]);

			if (pPrefixTxtRecord)
			{
				nCraftBonus += 3;
				nLevelRequirement = pPrefixTxtRecord->nClass != uint8_t(-1) && pPlayer && pPrefixTxtRecord->nClass == pPlayer->dwClassId ? pPrefixTxtRecord->nClassLevelReq : pPrefixTxtRecord->nLevelReq;
				if (nRequiredLevel <= nLevelRequirement)
				{
					nRequiredLevel = nLevelRequirement;
				}
			}

			if (pSuffixTxtRecord)
			{
				nCraftBonus += 3;
				nLevelRequirement = pSuffixTxtRecord->nClass != uint8_t(-1) && pPlayer && pSuffixTxtRecord->nClass == pPlayer->dwClassId ? pSuffixTxtRecord->nClassLevelReq : pSuffixTxtRecord->nLevelReq;
				if (nRequiredLevel <= nLevelRequirement)
				{
					nRequiredLevel = nLevelRequirement;
				}
			}
		}

		nRequiredLevel += nCraftBonus;

		if (nRequiredLevel >= DATATBLS_GetMaxLevel(0))
		{
			nRequiredLevel = DATATBLS_GetMaxLevel(0) - 1;
		}
		break;
	case ITEMQUAL_UNIQUE:
		if (pItemData->dwFileIndex >= 0 && pItemData->dwFileIndex < sgptDataTables->nUniqueItemsTxtRecordCount)
		{
			pUniqueItemsTxtRecord = &sgptDataTables->pUniqueItemsTxt[pItemData->dwFileIndex];
			if (pUniqueItemsTxtRecord)
			{
				if (pPlayer)
				{
					if (((pPlayer->dwFlagEx & UNITFLAGEX_ISEXPANSION) || ITEMS_GetItemFormat(pItem) >= 1) && (int)pUniqueItemsTxtRecord->wLvlReq >= 0)
					{
						nRequiredLevel = pUniqueItemsTxtRecord->wLvlReq;
					}
				}
				else
				{
					if ((int)pUniqueItemsTxtRecord->wLvlReq >= 0)
					{
						nRequiredLevel = pUniqueItemsTxtRecord->wLvlReq;
					}
				}
			}
		}
		break;
	case ITEMQUAL_SET:
		pSetItemsTxtRecord = ITEMS_GetSetItemsTxtRecord(ITEMS_GetFileIndex(pItem));
		if (pSetItemsTxtRecord && pSetItemsTxtRecord->wLvlReq >= 0)
		{
			nRequiredLevel = pSetItemsTxtRecord->wLvlReq;
		}
		break;
	default:
		break;
	}

	pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	if (pItemsTxtRecord)
	{
		if (pItemsTxtRecord->nLevelReq > nRequiredLevel)
		{
			nRequiredLevel = pItemsTxtRecord->nLevelReq;
		}
	}

	if (pItem->pInventory)
	{
		for (D2UnitStrc* i = INVENTORY_GetFirstItem(pItem->pInventory); i; i = INVENTORY_GetNextItem(i))
		{
			if (!INVENTORY_UnitIsItem(i))
			{
				break;
			}

			nRequiredLevelForSocketables = ITEMS_GetRequiredLevel(i, pPlayer);
			if (nRequiredLevel <= nRequiredLevelForSocketables)
			{
				nRequiredLevel = nRequiredLevelForSocketables;
			}
		}
	}

	for (int i = 0; i < STATLIST_CopyStats(pItem, STAT_ITEM_SINGLESKILL, pStat, ARRAY_SIZE(pStat)); ++i)
	{
		pSkillsTxtRecord = DATATBLS_GetSkillsTxtRecord(pStat[i].nLayer);
		if (pSkillsTxtRecord)
		{
			if (pSkillsTxtRecord->wReqLevel > nRequiredLevel)
			{
				nRequiredLevel = pSkillsTxtRecord->wReqLevel;
			}
		}
	}

	for (int i = 0; i < STATLIST_CopyStats(pItem, STAT_ITEM_NONCLASSSKILL, pStat, ARRAY_SIZE(pStat)); ++i)
	{
		pSkillsTxtRecord = DATATBLS_GetSkillsTxtRecord(pStat[i].nLayer);
		if (pSkillsTxtRecord)
		{
			if (!pPlayer || pPlayer->dwUnitType || pSkillsTxtRecord->nCharClass < 0 || pSkillsTxtRecord->nCharClass >= 7 || (pPlayer->dwClassId & 0xFF) != pSkillsTxtRecord->nCharClass)
			{
				nRequiredLevelForSkill = pSkillsTxtRecord->wReqLevel + 6;
			}
			else
			{
				nRequiredLevelForSkill = pSkillsTxtRecord->wReqLevel;
			}

			if (nRequiredLevelForSkill > nRequiredLevel)
			{
				nRequiredLevel = nRequiredLevelForSkill;
			}
		}
	}

	nRequiredLevel += STATLIST_UnitGetStatValue(pItem, STAT_ITEM_LEVELREQ, 0);
	if (nRequiredLevel <= 0)
	{
		nRequiredLevel = 0;
	}

	return nRequiredLevel;
}

// 1.13c D2Common+0x26DA0
//D2Common.0x6FD9A3F0 (#10757)
int __stdcall ITEMS_GetLevelRequirement(D2UnitStrc* pItem, D2UnitStrc* pUnit)
{
	return ITEMS_GetRequiredLevel(pItem, pUnit);
}

//D2Common.0x6FD9A400 (#10758)
// 1.13c D2Common+0x24C80
BOOL __stdcall ITEMS_CheckBodyLocation(D2UnitStrc* pItem, uint8_t nBodyLoc)
{
	if (!pItem || pItem->dwUnitType != UNIT_ITEM)
	{
		return FALSE;
	}

	// 1.13c: the two body locations of the item's type (0x23130, type from 0x230F0), 0/0 when the type is unknown
	uint8_t nBodyLoc1 = 0;
	uint8_t nBodyLoc2 = 0;
	const int32_t nType = (int32_t)ITEMS_GetItemType(pItem);
	if (nType >= 0 && nType < sgptDataTables->nItemTypesTxtRecordCount && sgptDataTables->pItemTypesTxt)
	{
		nBodyLoc1 = sgptDataTables->pItemTypesTxt[nType].nBodyLoc1;
		nBodyLoc2 = sgptDataTables->pItemTypesTxt[nType].nBodyLoc2;
	}

	if (nBodyLoc1 != nBodyLoc && nBodyLoc2 != nBodyLoc)
	{
		if (nBodyLoc != BODYLOC_SWRARM && nBodyLoc != BODYLOC_SWLARM)
		{
			return FALSE;
		}
		if (nBodyLoc1 != BODYLOC_RARM && nBodyLoc2 != BODYLOC_RARM && nBodyLoc1 != BODYLOC_LARM && nBodyLoc2 != BODYLOC_LARM)
		{
			return FALSE;
		}
	}

	return TRUE;
}

//D2Common.0x6FD9A4F0 (#10762)
// 1.13c D2Common+0x22F80
int __stdcall ITEMS_CheckItemTypeIfThrowable(int nItemType)
{
	if (nItemType >= 0 && nItemType < sgptDataTables->nItemTypesTxtRecordCount)
	{
		return sgptDataTables->pItemTypesTxt[nItemType].nThrowable;
	}

	return 0;
}

//D2Common.0x6FD9A530 (#10759)
// 1.13c D2Common+0x24C30
int __stdcall ITEMS_CheckIfThrowable(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		return ITEMS_CheckItemTypeIfThrowable((int)ITEMS_GetItemType(pItem));
	}

	return 0;
}

// 1.13c D2Common+0x22F50
//D2Common.0x6FD9A5B0 (#10760)
int __stdcall ITEMS_GetMissileType(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		return DATATBLS_GetItemsTxtRecord(pItem->dwClassId)->wMissileType;
	}

	return -1;
}

//D2Common.0x6FD9A5E0 (#10761)
// 1.13c D2Common+0x22F20
uint8_t __stdcall ITEMS_GetMeleeRange(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		return DATATBLS_GetItemsTxtRecord(pItem->dwClassId)->nRangeAdder;
	}

	return 0;
}

//D2Common.0x6FD9A610 (#10763)
// 1.13c D2Common+0x22EC0
BOOL __stdcall ITEMS_CheckWeaponClassByItemId(int nItemId, int nWeapClass)
{
	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);

	D2_ASSERT(pItemsTxtRecord);

	return pItemsTxtRecord->dwWeapClass == nWeapClass;
}

//D2Common.0x6FD9A660 (#10764)
// 1.13c D2Common+0x24C10
BOOL __stdcall ITEMS_CheckWeaponClass(D2UnitStrc* pItem, int nWeapClass)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		return ITEMS_CheckWeaponClassByItemId(pItem->dwClassId, nWeapClass);
	}

	return 0;
}

//D2Common.0x6FD9A6C0 (#10766)
// 1.13c D2Common+0x22E70
uint32_t __stdcall ITEMS_CheckWeaponIfTwoHandedByItemId(int nItemId)
{
	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);

	D2_ASSERT(pItemsTxtRecord);

	return pItemsTxtRecord->n2Handed;
}

//D2Common.0x6FD9A700 (#10765)
// 1.13c D2Common+0x24BF0
uint32_t __stdcall ITEMS_CheckWeaponIfTwoHanded(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		return ITEMS_CheckWeaponIfTwoHandedByItemId(pItem->dwClassId);
	}

	return 0;
}

//D2Common.0x6FD9A750 (#10767)
// 1.13c D2Common+0x22E20 (ordinal 10048)
uint32_t __stdcall ITEMS_CheckIfStackable(D2UnitStrc* pItem)
{
	D2ItemsTxt* pItemsTxtRecord = NULL;

	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		D2_ASSERT(pItemsTxtRecord);

		return pItemsTxtRecord->nStackable;
	}

	return 0;
}

// 1.13c D2Common+0x24BA0 (#10792)
//D2Common.0x6FD9A7A0 (#10768)
uint32_t __stdcall ITEMS_CheckIfBeltable(D2UnitStrc* pItem)
{
	D2ItemTypesTxt* pItemTypesTxtRecord = NULL;
	D2ItemsTxt* pItemsTxtRecord = NULL;

	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		D2_ASSERT(pItemsTxtRecord);

		if (pItemsTxtRecord->wType[0] >= 0 && pItemsTxtRecord->wType[0] < sgptDataTables->nItemTypesTxtRecordCount)
		{
			pItemTypesTxtRecord = &sgptDataTables->pItemTypesTxt[pItemsTxtRecord->wType[0]];
			if (pItemTypesTxtRecord)
			{
				return pItemTypesTxtRecord->nBeltable;
			}
		}
	}

	return 0;
}

//D2Common.0x6FD9A820 (#10769)
// 1.13c D2Common+0x22CD0
BOOL __stdcall ITEMS_ComparePotionTypes(D2UnitStrc* pItem1, D2UnitStrc* pItem2)
{
	static const int szHealthPotCodes[] =
	{
		' 1ph', ' 2ph', ' 3ph', ' 4ph', ' 5ph'
	};

	static const int szManaPotCodes[] =
	{
		' 1pm', ' 2pm', ' 3pm', ' 4pm', ' 5pm'
	};

	static const int szRejuvPotCodes[] =
	{
		' lvr', ' svr'
	};

	D2ItemsTxt* pItemsTxtRecord1 = NULL;
	D2ItemsTxt* pItemsTxtRecord2 = NULL;
	int nCounter = 0;
	bool bPot1 = false;
	bool bPot2 = false;

	if (pItem1 && pItem2)
	{
		if (pItem1->dwClassId == pItem2->dwClassId)
		{
			return TRUE;
		}

		pItemsTxtRecord1 = DATATBLS_GetItemsTxtRecord(pItem1->dwClassId);
		pItemsTxtRecord2 = DATATBLS_GetItemsTxtRecord(pItem2->dwClassId);

		bPot1 = false;
		bPot2 = false;
		nCounter = 0;
		do
		{
			if (!bPot1 && pItemsTxtRecord1->dwCode == szHealthPotCodes[nCounter])
			{
				bPot1 = true;
			}

			if (!bPot2 && pItemsTxtRecord2->dwCode == szHealthPotCodes[nCounter])
			{
				bPot2 = true;
			}

			if (bPot1 && bPot2)
			{
				return TRUE;
			}

			++nCounter;
		}
		while (nCounter < ARRAY_SIZE(szHealthPotCodes));


		bPot1 = false;
		bPot2 = false;
		nCounter = 0;
		do
		{
			if (!bPot1 && pItemsTxtRecord1->dwCode == szManaPotCodes[nCounter])
			{
				bPot1 = true;
			}

			if (!bPot2 && pItemsTxtRecord2->dwCode == szManaPotCodes[nCounter])
			{
				bPot2 = true;
			}

			if (bPot1 && bPot2)
			{
				return TRUE;
			}

			++nCounter;
		}
		while (nCounter < ARRAY_SIZE(szManaPotCodes));


		bPot1 = false;
		bPot2 = false;
		nCounter = 0;
		do
		{
			if (!bPot1 && pItemsTxtRecord1->dwCode == szRejuvPotCodes[nCounter])
			{
				bPot1 = true;
			}

			if (!bPot2 && pItemsTxtRecord2->dwCode == szRejuvPotCodes[nCounter])
			{
				bPot2 = true;
			}

			if (bPot1 && bPot2)
			{
				return TRUE;
			}

			++nCounter;
		}
		while (nCounter < ARRAY_SIZE(szRejuvPotCodes));

	}

	return FALSE;
}

//D2Common.0x6FD9A960 (#10770)
// 1.13c D2Common+0x24B20
BOOL __stdcall ITEMS_CheckIfAutoBeltable(D2InventoryStrc* pInventory, D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);
	return pItemsTxtRecord->nAutoBelt || INVENTORY_HasSimilarPotionInBelt(pInventory, pItem) != -1;
}

//D2Common.0x6FD9AA00 (#10771)
// 1.13c D2Common+0x22C80: a non-item answers 0 without the 1.10f trace.
BOOL __stdcall ITEMS_CheckIfUseable(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		D2_ASSERT(pItemsTxtRecord);

		return pItemsTxtRecord->nUseable;
	}

	return 0;
}

//D2Common.0x6FD9AA70 (#10772)
// 1.13c D2Common+0x22C20
int __stdcall ITEMS_GetUniqueColumnFromItemsTxt(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);
	return pItemsTxtRecord->nUnique;
}

//D2Common.0x6FD9AB00 (#10773)
// 1.13c D2Common+0x22BC0
BOOL __stdcall ITEMS_IsQuestItem(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);
	return pItemsTxtRecord->nQuest;
}

//D2Common.0x6FD9AB90
// 1.13c D2Common+0x25810
int __fastcall ITEMS_CalculateAdditionalCostsForChargedSkills(D2UnitStrc* pUnit, int nBaseCost)
{
	D2SkillsTxt* pSkillsTxtRecord = NULL;
	int nCounter = 0;
	int nSkillId = 0;
	int nValue = 0;
	int nStats = 0;
	int nCost = 0;
	int nBase = 0;
	int nAdd = 0;
	int nMax = 0;
	D2StatStrc pStat[64] = {};

	nStats = STATLIST_CopyStats(pUnit, STAT_ITEM_CHARGED_SKILL, pStat, ARRAY_SIZE(pStat));
	if (nStats > 0)
	{
		do
		{
			nSkillId = pStat[nCounter].nLayer >> sgptDataTables->nStuff;

			nMax = pStat[nCounter].nValue >> 8;
			nValue = pStat[nCounter].nValue & 0xFF;

			if (nValue < nMax)
			{
				pSkillsTxtRecord = DATATBLS_GetSkillsTxtRecord(nSkillId);
				if (pSkillsTxtRecord)
				{
					nBase = nBaseCost * ((pStat[nCounter].nLayer & sgptDataTables->nShiftedStuff) + ((int32_t)(int16_t)pSkillsTxtRecord->wReqLevel / 6) * 2 + 2);	// 1.13c: (req level / 6) * 2, not 2 * level / 6
					if (nBase > 65535 && pSkillsTxtRecord->nCostMult)
					{
						nAdd = nBase / 1024 * pSkillsTxtRecord->nCostMult;	// 1.13c: divide first for large operands
					}
					else
					{
						nAdd = nBase * pSkillsTxtRecord->nCostMult / 1024;
					}

					nCost += (pSkillsTxtRecord->nCostAdd + nAdd) * (nMax - nValue) / nMax;
				}
			}
			++nCounter;
		}
		while (nCounter < nStats);

		return nCost;
	}

	return 0;
}

//D2Common.0x6FD9ACE0
// 1.13c D2Common+0x28850: with the operand over 0xFFFF the quotient is taken first and then scaled by the multiplier
void __fastcall ITEMS_CalculateAdditionalCostsForBonusStats(D2UnitStrc* pItem, int* pSellCost, int* pBuyCost, int* pRepCost, unsigned int nDivisor)
{
	D2ItemStatCostTxt* pItemStatCostTxtRecord = NULL;
	D2SkillsTxt* pSkillsTxtRecord = NULL;
	unsigned int nSell = 0;
	unsigned int nBuy = 0;
	unsigned int nRep = 0;
	int nBonusValue = 0;
	int nSkillId = 0;
	int nCounter = 0;
	int nStatId = 0;
	int nStats = 0;
	int nValue = 0;
	int nItemPeriodOfDay = 0;
	int nMinValue = 0;
	int nMaxValue = 0;
	D2StatStrc pStat[511] = {};

	nStats = STATLIST_GetFullStatsDataFromUnit(pItem, pStat, ARRAY_SIZE(pStat));

	while (nCounter < nStats)
	{
		nStatId = pStat[nCounter].nStat;
		nBonusValue = STATLIST_GetUnitStatBonus(pItem, nStatId, pStat[nCounter].nLayer);

		if (nBonusValue)
		{
			pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nStatId);
			if (pItemStatCostTxtRecord)
			{
				if (pItemStatCostTxtRecord->nValShift)
				{
					nBonusValue >>= pItemStatCostTxtRecord->nValShift;
				}

				switch (pItemStatCostTxtRecord->nEncode)
				{
				case 1:
					pSkillsTxtRecord = DATATBLS_GetSkillsTxtRecord(pStat[nCounter].nLayer);
					if (pSkillsTxtRecord)
					{
						if (nBonusValue * *pSellCost > 65535 && pSkillsTxtRecord->nCostMult)
						{
							nSell += pSkillsTxtRecord->nCostAdd + nBonusValue * *pSellCost / 1024 * pSkillsTxtRecord->nCostMult;
							nBuy += pSkillsTxtRecord->nCostAdd + nBonusValue * *pBuyCost / 4096 * pSkillsTxtRecord->nCostMult;
							nRep += pSkillsTxtRecord->nCostAdd + nBonusValue * *pRepCost / 1024 * pSkillsTxtRecord->nCostMult;
						}
						else
						{
							nSell += pSkillsTxtRecord->nCostAdd + nBonusValue * *pSellCost * pSkillsTxtRecord->nCostMult / 1024;
							nBuy += pSkillsTxtRecord->nCostAdd + nBonusValue * pSkillsTxtRecord->nCostMult * *pBuyCost / 4096;
							nRep += pSkillsTxtRecord->nCostAdd + nBonusValue * pSkillsTxtRecord->nCostMult * *pRepCost / 1024;
						}
					}
					break;
				case 2:
				case 3:
					nSkillId = pStat[nCounter].nLayer >> sgptDataTables->nStuff;
					nValue = pStat[nCounter].nLayer & sgptDataTables->nShiftedStuff;

					pSkillsTxtRecord = DATATBLS_GetSkillsTxtRecord(nSkillId);

					if (pSkillsTxtRecord)
					{
						if (nValue * *pSellCost > 65535 && pSkillsTxtRecord->nCostMult)
						{
							nSell += pSkillsTxtRecord->nCostAdd + nValue * *pSellCost / 1024 * pSkillsTxtRecord->nCostMult;
							nBuy += pSkillsTxtRecord->nCostAdd + nValue * *pBuyCost / 4096 * pSkillsTxtRecord->nCostMult;
							nRep += pSkillsTxtRecord->nCostAdd + nValue * *pRepCost / 1024 * pSkillsTxtRecord->nCostMult;
						}
						else
						{
							nSell += pSkillsTxtRecord->nCostAdd + nValue * *pSellCost * pSkillsTxtRecord->nCostMult / 1024;
							nBuy += pSkillsTxtRecord->nCostAdd + nValue * pSkillsTxtRecord->nCostMult * *pBuyCost / 4096;
							nRep += pSkillsTxtRecord->nCostAdd + nValue * pSkillsTxtRecord->nCostMult * *pRepCost / 1024;
						}
					}
					break;
				case 4:
					ITEMMODS_GetByTimeAdjustment(nBonusValue, 0, 0, &nItemPeriodOfDay, &nMinValue, &nMaxValue);

					nValue = (nMinValue + nMaxValue) / 2;

					if (nValue * *pSellCost > 65535 && pItemStatCostTxtRecord->dwMultiply)
					{
						nSell += pItemStatCostTxtRecord->dwAdd + nValue * *pSellCost / 1024 * pItemStatCostTxtRecord->dwMultiply;
						nBuy += pItemStatCostTxtRecord->dwAdd + nValue * *pBuyCost / 1024 * pItemStatCostTxtRecord->dwMultiply;
						nRep += pItemStatCostTxtRecord->dwAdd + nValue * *pRepCost / 1024 * pItemStatCostTxtRecord->dwMultiply;
					}
					else
					{
						nSell += pItemStatCostTxtRecord->dwAdd + nValue * *pSellCost * pItemStatCostTxtRecord->dwMultiply / 1024;
						nBuy += pItemStatCostTxtRecord->dwAdd + nValue * pItemStatCostTxtRecord->dwMultiply * *pBuyCost / 1024;
						nRep += pItemStatCostTxtRecord->dwAdd + nValue * pItemStatCostTxtRecord->dwMultiply * *pRepCost / 1024;
					}
					break;
				default:
					if (nBonusValue * *pSellCost > 65535 && pItemStatCostTxtRecord->dwMultiply)
					{
						nSell += pItemStatCostTxtRecord->dwAdd + nBonusValue * *pSellCost / 1024 * pItemStatCostTxtRecord->dwMultiply;
						nBuy += pItemStatCostTxtRecord->dwAdd + nBonusValue * *pBuyCost / 1024 * pItemStatCostTxtRecord->dwMultiply;
						nRep += pItemStatCostTxtRecord->dwAdd + nBonusValue * *pRepCost / 1024 * pItemStatCostTxtRecord->dwMultiply;
					}
					else
					{
						nSell += pItemStatCostTxtRecord->dwAdd + nBonusValue * *pSellCost * pItemStatCostTxtRecord->dwMultiply / 1024;
						nBuy += pItemStatCostTxtRecord->dwAdd + nBonusValue * pItemStatCostTxtRecord->dwMultiply * *pBuyCost / 1024;
						nRep += pItemStatCostTxtRecord->dwAdd + nBonusValue * pItemStatCostTxtRecord->dwMultiply * *pRepCost / 1024;
					}
					break;
				}
			}
		}

		++nCounter;
	}

	*pSellCost += nSell / nDivisor;
	*pBuyCost += nBuy / nDivisor;
	*pRepCost += nRep / nDivisor;
}

// 1.13c D2Common+0x28E80: retail's price scaling. Large operands (gate >= 0x10000 with a non-zero gate
// multiplier) divide by 1024 first, then multiply; small ones multiply first. Signed, 32-bit wrap.
static int D2Price_Scale(int nCost, int nGate, uint32_t nGateMult, uint32_t nMult)
{
	if (nGate < 0x10000 || nGateMult == 0)
	{
		return (int)((uint32_t)nCost * nMult) / 1024;
	}
	return (int)((uint32_t)(nCost / 1024) * nMult);
}

//D2Common.0x6FD9B1C0 -- 1.13c D2Common+0x28E80 (EAX = pPlayer, 5 stack args; callers 0x29D60, 0x2A210)
int __fastcall ITEMS_CalculateTransactionCost(D2UnitStrc* pPlayer, D2UnitStrc* pItem, D2C_Difficulties nDifficulty, D2BitBufferStrc* pQuestFlags, int nVendorId, D2C_TransactionTypes nTransactionType)
{
	D2UniqueItemsTxt* pUniqueItemsTxtRecord = NULL;
	D2MagicAffixTxt* pMagicAffixTxtRecord = NULL;
	D2ItemTypesTxt* pItemTypesTxtRecord = NULL;
	D2SetItemsTxt* pSetItemsTxtRecord = NULL;
	D2MonStatsTxt* pMonStatsTxtRecord = NULL;
	D2ItemsTxt* pUltraItemsTxtRecord = NULL;
	D2ItemsTxt* pUberItemsTxtRecord = NULL;
	D2ItemsTxt* pItemsTxtRecord = NULL;
	D2BooksTxt* pBooksTxtRecord = NULL;
	D2NpcTxt* pNpcTxtRecord = NULL;
	D2UnitStrc* pSocketable = NULL;
	int nSocketableCost = 0;
	int nReducePricePct = 0;
	int nStackQuantity = 0;
	int nArmorClass = 0;
	int nDurability = 0;
	int nFileIndex = 0;
	int nUltraCost = 0;
	int nUberCost = 0;
	int nItemType = 0;
	int nQuantity = 0;
	int nMaxStack = 0;
	int nSellCost = 0;
	int nBuyCost = 0;
	int nRepCost = 0;
	int nDivisor = 0;
	int nMaxDura = 0;
	int nQuality = 0;
	int nItemId = 0;
	int nLevel = 0;
	int nStack = 0;
	int nCost = 0;
	int nBase = 0;
	int nSell = 0;
	int nBuy = 0;
	int nRep = 0;

	int v30 = 0, v27 = 0, v22 = 0, v23 = 0; //TODO: Rename
	D2ItemDataStrc* pItemData = ITEMS_GetItemData(pItem);
	if (!pPlayer || !pItemData)
	{
		return 0x7FFFFFFF;
	}

	nLevel = STATLIST_UnitGetStatValue(pPlayer, STAT_LEVEL, 0);

	if (nTransactionType == D2C_TransactionTypes::TRANSACTIONTYPE_REPAIR && !ITEMS_IsRepairable(pItem))
	{
		return 0;
	}

	if (pItemData->dwItemFlags & IFLAG_STARTITEM)
	{
		return 1;
	}

	pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	nQuantity = STATLIST_UnitGetStatValue(pItem, STAT_QUANTITY, 0);
	if (nQuantity <= 0)
	{
		nQuantity = 1;
	}
	
	nReducePricePct = STATLIST_UnitGetStatValue(pPlayer, STAT_ITEM_REDUCEDPRICES, 0);
	if (nReducePricePct >= 99)
	{
		nReducePricePct = 99;
	}

	if (nTransactionType != D2C_TransactionTypes::TRANSACTIONTYPE_GAMBLE)
	{
		nSellCost = 0;
		nBuyCost = 0;
		nRepCost = 0;
		nDivisor = 1;

		if (pItemData->dwItemFlags & IFLAG_ISEAR)
		{
			nSellCost = pItemsTxtRecord->dwCost * pItemData->nEarLvl;
			nBuyCost = nSellCost;
		}
		else
		{
			if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_BODY_PART))
			{
				nFileIndex = pItemData->dwFileIndex;
				pMonStatsTxtRecord = DATATBLS_GetMonStatsTxtRecord(nFileIndex);
				if (pMonStatsTxtRecord)
				{
					nSellCost = 8 * pMonStatsTxtRecord->nLevel[nDifficulty];
				}

				nSellCost += pItemsTxtRecord->dwCost;
				nBuyCost = nSellCost;
			}
			else
			{
				if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_BOOK))
				{
					pBooksTxtRecord = DATATBLS_GetBooksTxtRecord(pItemData->wMagicSuffix[0]);
					D2_ASSERT(pBooksTxtRecord);

					nSellCost = pItemsTxtRecord->dwCost + nQuantity * pBooksTxtRecord->dwCostPerCharge;
					nBuyCost = nSellCost;
				}
				else
				{
					pItemTypesTxtRecord = DATATBLS_GetItemTypesTxtRecord(ITEMS_GetItemTypeFromItemId(pItem->dwClassId));

					if (pItemTypesTxtRecord && pItemTypesTxtRecord->wQuiver)
					{
						nSellCost = nQuantity * pItemsTxtRecord->dwCost / 1024;
						nBuyCost = nSellCost;

						nStackQuantity = STATLIST_UnitGetStatValue(pItem, STAT_ITEM_EXTRA_STACK, 0) + pItemsTxtRecord->dwMaxStack;
						if (nStackQuantity >= 511)
						{
							nStackQuantity = 511;
						}

						nRepCost = nStackQuantity * pItemsTxtRecord->dwCost / 1024;
					}
					else
					{
						if (!pItemsTxtRecord->nStackable)
						{
							nSellCost = pItemsTxtRecord->dwCost;
							nBuyCost = nSellCost;
							nRepCost = nSellCost;
						}
						else
						{
							nSellCost = pItemsTxtRecord->dwCost;
							nBuyCost = nSellCost;
							nRepCost = nSellCost;

							nDivisor = STATLIST_UnitGetStatValue(pItem, STAT_ITEM_EXTRA_STACK, 0) + pItemsTxtRecord->dwMaxStack;

							if (nDivisor < 511)
							{
								if (nDivisor <= 1)
								{
									nDivisor = 1;
								}
							}
							else
							{
								nDivisor = 511;
							}
						}
					}
				}
			}
		}

		if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_ANY_ARMOR))
		{
			nArmorClass = STATLIST_GetUnitBaseStat(pItem, STAT_ARMORCLASS, 0);
			if (pItemsTxtRecord->dwMaxAc - pItemsTxtRecord->dwMinAc != -1)
			{
				if (pItemsTxtRecord->dwMaxAc)
				{
					nSellCost = nArmorClass * pItemsTxtRecord->dwCost / pItemsTxtRecord->dwMaxAc;
					nBuyCost = nArmorClass * pItemsTxtRecord->dwCost / pItemsTxtRecord->dwMaxAc;
					nRepCost = nArmorClass * pItemsTxtRecord->dwCost / pItemsTxtRecord->dwMaxAc;
				}
			}
		}

		nQuality = pItemData->dwQualityNo;

		if (!ITEMS_IsMagSetRarUniCrfOrTmp(pItem))
		{
			ITEMS_CalculateAdditionalCostsForItemSkill(pItem, &nSellCost, &nBuyCost, &nRepCost, nDivisor);
		}

		if (pItemData->dwItemFlags & IFLAG_IDENTIFIED)
		{
			// 1.13c: every affix term is add + scale(cost, mult); the sell cost gates all three scalings
			auto AddTerm = [&](uint32_t nMult, uint32_t nAdd)
			{
				nSell += nAdd + D2Price_Scale(nSellCost, nSellCost, nMult, nMult);
				nBuy += nAdd + D2Price_Scale(nBuyCost, nSellCost, nMult, nMult);
				nRep += nAdd + D2Price_Scale(nRepCost, nSellCost, nMult, nMult);
			};
			auto AddAffix = [&](uint16_t wAffix)
			{
				if (D2MagicAffixTxt* pAffix = DATATBLS_GetMagicAffixTxtRecord(wAffix))
				{
					AddTerm(pAffix->dwMultiply, pAffix->dwAdd);
				}
			};

			AddAffix(pItemData->wAutoAffix);

			switch (nQuality)
			{
			case ITEMQUAL_RARE:
			case ITEMQUAL_CRAFT:
				for (int i = 0; i < ITEMS_MAX_MODS; ++i)
				{
					AddAffix(pItemData->wMagicPrefix[i]);
					AddAffix(pItemData->wMagicSuffix[i]);
				}
				ITEMS_CalculateAdditionalCostsForBonusStats(pItem, &nSellCost, &nBuyCost, &nRepCost, nDivisor);
				break;

			case ITEMQUAL_UNIQUE:
				// 1.13c: a found unique row ends the case; a missing row (or negative index) continues as magic
				if (pItemData->dwFileIndex >= 0 && pItemData->dwFileIndex < sgptDataTables->nUniqueItemsTxtRecordCount)
				{
					pUniqueItemsTxtRecord = &sgptDataTables->pUniqueItemsTxt[pItemData->dwFileIndex];
					AddTerm(pUniqueItemsTxtRecord->dwCostMult, pUniqueItemsTxtRecord->dwCostAdd);
					break;
				}
				[[fallthrough]];

			case ITEMQUAL_MAGIC:
				AddAffix(pItemData->wMagicPrefix[0]);
				AddAffix(pItemData->wMagicSuffix[0]);
				ITEMS_CalculateAdditionalCostsForBonusStats(pItem, &nSellCost, &nBuyCost, &nRepCost, nDivisor);
				break;

			case ITEMQUAL_SUPERIOR:
			case ITEMQUAL_TEMPERED:
				ITEMS_CalculateAdditionalCostsForBonusStats(pItem, &nSellCost, &nBuyCost, &nRepCost, nDivisor);
				break;

			case ITEMQUAL_SET:
				if (pItemData->dwFileIndex >= 0 && pItemData->dwFileIndex < sgptDataTables->nSetItemsTxtRecordCount)
				{
					pSetItemsTxtRecord = &sgptDataTables->pSetItemsTxt[pItemData->dwFileIndex];
					AddTerm(pSetItemsTxtRecord->dwCostMult, pSetItemsTxtRecord->dwCostAdd);
				}
				break;

			case ITEMQUAL_INFERIOR:
				nSell = nSellCost / -2;
				nBuy = nBuyCost / -2;
				nRep = nRepCost / -2;
				break;

			default:
				break;
			}

			nSellCost += nSell / nDivisor;
			nBuyCost += nBuy / nDivisor;
			nRepCost += nRep / nDivisor;

			if (ITEMS_IsMagSetRarUniCrfOrTmp(pItem))
			{
				ITEMS_CalculateAdditionalCostsForItemSkill(pItem, &nSellCost, &nBuyCost, &nRepCost, nDivisor);
			}
		}

		if (pItem->pInventory)
		{
			pSocketable = INVENTORY_GetFirstItem(pItem->pInventory);
			while (pSocketable)
			{
				if (INVENTORY_UnitIsItem(pSocketable))
				{
					nSocketableCost = DATATBLS_GetItemsTxtRecord(pSocketable->dwClassId)->dwCost / 2;
					nSellCost += nSocketableCost;
					nBuyCost += nSocketableCost;
					nRepCost += nSocketableCost;
				}
				pSocketable = INVENTORY_GetNextItem(pSocketable);
			}
		}

		if (pItemData->dwItemFlags & IFLAG_ETHEREAL)
		{
			nBuyCost /= 4;
		}

		// 1.13c D2Common+0x28E80: the item-type row of the class id (+0x0E quiver word, +0x10 throwable byte, +0x21 class)
		D2ItemTypesTxt* pTypeRec = NULL;
		nItemType = ITEMS_GetItemTypeFromItemId(pItem->dwClassId);
		if (nItemType >= 0 && nItemType < sgptDataTables->nItemTypesTxtRecordCount)
		{
			pTypeRec = &sgptDataTables->pItemTypesTxt[nItemType];
			if (pTypeRec->nClass < 7)
			{
				nBuyCost /= 4;
			}
		}

		if (nTransactionType == D2C_TransactionTypes::TRANSACTIONTYPE_SELL)
		{
			if (ITEMS_IsEthereal(pItem) && ITEMS_HasDurability(pItem) && STATLIST_GetMaxDurabilityFromUnit(pItem) && STATLIST_UnitGetStatValue(pItem, STAT_DURABILITY, 0) < 1)
			{
				nBuyCost = 0;
			}
		}
		else if (nTransactionType == D2C_TransactionTypes::TRANSACTIONTYPE_REPAIR)
		{
			// 1.13c: not a quiver type, not a throwable type, has durability; prorate by (max - cur) with the multiply first
			if ((!pTypeRec || !pTypeRec->wQuiver) && (!pTypeRec || !pTypeRec->nThrowable) && ITEMS_HasDurability(pItem))
			{
				nDurability = STATLIST_UnitGetStatValue(pItem, STAT_DURABILITY, 0);
				nMaxDura = STATLIST_GetMaxDurabilityFromUnit(pItem);
				if (nMaxDura == 0 || nMaxDura <= nDurability)
				{
					nRepCost = 0;
				}
				else
				{
					const bool bReplenish = STATLIST_UnitGetStatValue(pItem, STAT_ITEM_REPLENISH_DURABILITY, 0) != 0;
					const int nMissing = bReplenish ? nMaxDura - 1 : nMaxDura - nDurability;
					nRepCost = (bReplenish && nMissing <= nDurability) ? 0 : nMissing * nRepCost / nMaxDura;
				}
			}
		}

		pNpcTxtRecord = DATATBLS_GetNpcTxtRecord(nVendorId);
		D2_ASSERT(pNpcTxtRecord);

		// 1.13c: the npc.txt multipliers; the buy gate tests the sell multiplier (retail quirk, kept)
		nSellCost = D2Price_Scale(nSellCost, nSellCost, pNpcTxtRecord->dwSellMult, pNpcTxtRecord->dwSellMult);
		nBuyCost = D2Price_Scale(nBuyCost, nBuyCost, pNpcTxtRecord->dwSellMult, pNpcTxtRecord->dwBuyMult);
		nRepCost = D2Price_Scale(nRepCost, nRepCost, pNpcTxtRecord->dwRepMult, pNpcTxtRecord->dwRepMult);

		for (int i = 0; i < 3; ++i)
		{
			if (pNpcTxtRecord->dwQuestFlag[i] && (QUESTRECORD_GetQuestState(pQuestFlags, pNpcTxtRecord->dwQuestFlag[i], QFLAG_REWARDGRANTED) == 1
												  || QUESTRECORD_GetQuestState(pQuestFlags, pNpcTxtRecord->dwQuestFlag[i], QFLAG_REWARDPENDING) == 1))
			{
				nSellCost = D2Price_Scale(nSellCost, nSellCost, pNpcTxtRecord->dwQuestSellMult[i], pNpcTxtRecord->dwQuestSellMult[i]);
				nBuyCost = D2Price_Scale(nBuyCost, nBuyCost, pNpcTxtRecord->dwQuestBuyMult[i], pNpcTxtRecord->dwQuestBuyMult[i]);
				nRepCost = D2Price_Scale(nRepCost, nRepCost, pNpcTxtRecord->dwQuestRepMult[i], pNpcTxtRecord->dwQuestRepMult[i]);
			}
		}

		if (!ITEMS_CheckItemTypeId(pItem, ITEMTYPE_BOOK) && (!pTypeRec || !pTypeRec->wQuiver))
		{
			nSellCost *= nQuantity;

			if (ITEMS_CheckIfStackable(pItem) && ITEMS_IsRepairable(pItem))
			{
				nMaxStack = STATLIST_UnitGetStatValue(pItem, STAT_ITEM_EXTRA_STACK, 0) + pItemsTxtRecord->dwMaxStack;
				if (nMaxStack >= 511)
				{
					nMaxStack = 511;
				}

				if (nMaxStack <= nQuantity || STATLIST_UnitGetStatValue(pItem, STAT_ITEM_REPLENISH_QUANTITY, 0))
				{
					nRepCost = 0;
					nBuyCost = nMaxStack * nBuyCost;
				}
				else
				{
					nRepCost *= (nMaxStack - nQuantity);
					nBuyCost = nMaxStack * nBuyCost - nRepCost;
				}
			}
			else
			{
				nBuyCost *= nQuantity;
			}
		}

		if (nTransactionType == D2C_TransactionTypes::TRANSACTIONTYPE_REPAIR && !ITEMS_IsEthereal(pItem))
		{
			nRepCost += ITEMS_CalculateAdditionalCostsForChargedSkills(pItem, 10000);
		}

		nCost = nBuyCost;
		if (pNpcTxtRecord->nMaxBuy[nDifficulty] < nCost)
		{
			nCost = pNpcTxtRecord->nMaxBuy[nDifficulty];
		}

		if (nTransactionType != D2C_TransactionTypes::TRANSACTIONTYPE_SELL)
		{
			nCost = nTransactionType == D2C_TransactionTypes::TRANSACTIONTYPE_REPAIR ? nRepCost : nSellCost;
			if (nReducePricePct)
			{
				nCost -= DATATBLS_ApplyRatio(nCost, nReducePricePct, 100);
			}
		}

		if (nCost < 1)
		{
			nCost = 1;
		}

		return nCost;
	}
	else
	{
		if (pItemData->wItemFormat >= 1)
		{
			pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(DATATBLS_GetItemIdFromItemCode(pItemsTxtRecord->dwNormCode ? pItemsTxtRecord->dwNormCode : pItemsTxtRecord->dwCode));
			D2_ASSERT(pItemsTxtRecord);

			if (pItemsTxtRecord->dwCode == ' nir' || pItemsTxtRecord->dwCode == ' uma')
			{
				nCost = pItemsTxtRecord->dwGambleCost;
			}
			else
			{
				nStack = pItemsTxtRecord->dwMinStack + pItemsTxtRecord->dwMaxStack;

				v27 = (int)nStack / 2;   // d2-web: the decompiler HIDWORD/BYTE4 forms are x86 signed-division idioms (cdq; sub; sar); kind 21 phase-2 vectors (s84) price gambles against retail
				if (v27 < 1)
				{
					v27 = 1;
				}

				if (pItemsTxtRecord->dwUberCode && pItemsTxtRecord->dwUberCode != '   0')
				{
					pUberItemsTxtRecord = DATATBLS_GetItemRecordFromItemCode(pItemsTxtRecord->dwUberCode, &nItemId);
					if (pUberItemsTxtRecord)
					{
						v30 = 100 * (nLevel - pUberItemsTxtRecord->nLevel);
						v23 = (int)v30 / 2 + 1;   // d2-web: signed division (see v27)
						if (v23 < 0)
						{
							v23 = 0;
						}
						nUberCost = pUberItemsTxtRecord->dwCost;
					}
				}

				if (pItemsTxtRecord->dwUltraCode && pItemsTxtRecord->dwUltraCode != '   0')
				{
					pUltraItemsTxtRecord = DATATBLS_GetItemRecordFromItemCode(pItemsTxtRecord->dwUltraCode, &nItemId);
					if (pUltraItemsTxtRecord)
					{
						v30 = 100 * (nLevel - pUltraItemsTxtRecord->nLevel);
						v22 = (int)v30 / 4 + 1;   // d2-web: signed division (see v27)
						if (v22 < 0)
						{
							v22 = 0;
						}
						nUltraCost = pUltraItemsTxtRecord->dwCost;
					}
				}

				nBase = v23 * nUberCost + v22 * nUltraCost + (v27 * pItemsTxtRecord->dwCost) * (10000 - v22 - v23);
				if (nLevel < 5)
				{
					nLevel = 5;
				}

				nCost = ((2 * nLevel + 1) / 3 + 20) * (nBase / 10000 + (250 * (nLevel + ((pItemsTxtRecord->nLevel - 45) > 0 ? (pItemsTxtRecord->nLevel - 45) : 0) - (int)pItemsTxtRecord->nLevel / 2)) / 3) / 15;
			}

			if (nReducePricePct)
			{
				return nCost - DATATBLS_ApplyRatio(nCost, nReducePricePct, 100);
			}
			else
			{
				return nCost;
			}
		}
		else
		{
			return DATATBLS_GetItemsTxtRecord(DATATBLS_GetItemIdFromItemCode(pItemsTxtRecord->dwNormCode ? pItemsTxtRecord->dwNormCode : pItemsTxtRecord->dwCode))->dwGambleCost;
		}
	}
}

//D2Common.0x6FD9CB50
// 1.13c D2Common+0x25970
void __fastcall ITEMS_CalculateAdditionalCostsForItemSkill(D2UnitStrc* pItem, int* pSellCost, int* pBuyCost, int* pRepCost, unsigned int nDivisor)
{
	D2ItemTypesTxt* pItemTypesTxtRecord = NULL;
	D2SkillsTxt* pSkillsTxtRecord = NULL;
	D2ItemsTxt* pItemsTxtRecord = NULL;
	unsigned int nSell = 0;
	unsigned int nBuy = 0;
	unsigned int nRep = 0;
	int nStats = 0;
	D2StatStrc pStat[64] = {};

	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		D2_ASSERT(pItemsTxtRecord);

		if (pItemsTxtRecord->wType[0] >= 0 && pItemsTxtRecord->wType[0] < sgptDataTables->nItemTypesTxtRecordCount)
		{
			pItemTypesTxtRecord = &sgptDataTables->pItemTypesTxt[pItemsTxtRecord->wType[0]];
			if (pItemTypesTxtRecord && pItemTypesTxtRecord->nStaffMods != 7)
			{
				nStats = STATLIST_CopyStats(pItem, STAT_ITEM_SINGLESKILL, pStat, ARRAY_SIZE(pStat));

				for (int nStatId = 0; nStatId < nStats; ++nStatId)
				{
					pSkillsTxtRecord = DATATBLS_GetSkillsTxtRecord(pStat[nStatId].nLayer);
					if (pSkillsTxtRecord)
					{
						if (*pSellCost > 65535 && pSkillsTxtRecord->nCostMult)
						{	// 1.13c: the multiplier is divided first for large operands
							nSell += (2 * pStat[nStatId].nValue - 1) * (pSkillsTxtRecord->nCostAdd + pSkillsTxtRecord->nCostMult / 1024 * *pSellCost);
							nBuy += (2 * pStat[nStatId].nValue - 1) * (pSkillsTxtRecord->nCostAdd + pSkillsTxtRecord->nCostMult / 4096 * *pBuyCost);
							nRep += (2 * pStat[nStatId].nValue - 1) * (pSkillsTxtRecord->nCostAdd + pSkillsTxtRecord->nCostMult / 1024 * *pRepCost);
						}
						else
						{
							nSell += (2 * pStat[nStatId].nValue - 1) * (pSkillsTxtRecord->nCostAdd + pSkillsTxtRecord->nCostMult * *pSellCost / 1024);
							nBuy += (2 * pStat[nStatId].nValue - 1) * (pSkillsTxtRecord->nCostAdd + pSkillsTxtRecord->nCostMult * *pBuyCost / 4096);
							nRep += (2 * pStat[nStatId].nValue - 1) * (pSkillsTxtRecord->nCostAdd + pSkillsTxtRecord->nCostMult * *pRepCost / 1024);
						}
					}
				}

				*pSellCost += nSell / nDivisor;
				*pBuyCost += nBuy / nDivisor;
				*pRepCost += nRep / nDivisor;
			}
		}
	}
}

//D2Common.0x6FD9CDE0 (#10775)
// 1.13c D2Common+0x29D60
int __stdcall ITEMS_GetTransactionCost(D2UnitStrc* pPlayer, D2UnitStrc* pItem, D2C_Difficulties nDifficulty, D2BitBufferStrc* pQuestFlags, int nVendorId, D2C_TransactionTypes nTransactionType)
{
	return ITEMS_CalculateTransactionCost(pPlayer, pItem, nDifficulty, pQuestFlags, nVendorId, nTransactionType);
}

//D2Common.0x6FD9CE10 (#10794)
// 1.13c D2Common+0x22B70
int __stdcall ITEMS_GetMaxStack(int nItemId)
{
	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);

	D2_ASSERT(pItemsTxtRecord);

	return pItemsTxtRecord->dwMaxStack;
}

//D2Common.0x6FD9CE50 (#10795)
// 1.13c D2Common+0x260F0
int __stdcall ITEMS_GetTotalMaxStack(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	// retail: items.txt max stack (0x22B70), then the extra-stack stat, clamped to 511
	int nMaxStack = ITEMS_GetMaxStack(pItem->dwClassId);
	nMaxStack += STATLIST_UnitGetStatValue(pItem, STAT_ITEM_EXTRA_STACK, 0);
	if (nMaxStack >= 511)
	{
		nMaxStack = 511;
	}

	return nMaxStack;
}

//D2Common.0x6FD9CEF0 (#10798)
// 1.13c D2Common+0x22B20
int __stdcall ITEMS_GetSpawnStackFromItemId(int nItemId)
{
	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);

	D2_ASSERT(pItemsTxtRecord);

	return pItemsTxtRecord->dwSpawnStack;
}

//D2Common.0x6FD9CF30 (#10799)
// 1.13c D2Common+0x24800
int __stdcall ITEMS_GetSpawnStack(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	return ITEMS_GetSpawnStackFromItemId(pItem->dwClassId);
}

//D2Common.0x6FD9CFB0 (#10796)
// 1.13c D2Common+0x22AD0
int __stdcall ITEMS_GetMinStackFromItemId(int nItemId)
{
	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);

	D2_ASSERT(pItemsTxtRecord);

	return pItemsTxtRecord->dwMinStack;
}

//D2Common.0x6FD9CFF0 (#10797)
// 1.13c D2Common+0x247C0
int __stdcall ITEMS_GetMinStack(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	return ITEMS_GetMinStackFromItemId(pItem->dwClassId);
}

//D2Common.0x6FD9D070 (#10800) - Unused
// 1.13c D2Common+0x22A70
int __stdcall ITEMS_CheckBitField1Flag8(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);
	return pItemsTxtRecord->dwBitField1 & 8;
}

//D2Common.0x6FD9D0F0 (#10804)
// 1.13c D2Common+0x24710
int __stdcall ITEMS_GetSpellIcon(D2UnitStrc* pItem)
{
	D2BooksTxt* pBooksTxtRecord = NULL;
	uint16_t wBookId = 0;

	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_SCROLL) || ITEMS_CheckItemTypeId(pItem, ITEMTYPE_BOOK))
	{
		if (D2ItemDataStrc* pItemData = ITEMS_GetItemData(pItem))
		{
			wBookId = pItemData->wMagicSuffix[0];
		}
		else
		{
			wBookId = 0;
		}

		pBooksTxtRecord = DATATBLS_GetBooksTxtRecord(wBookId);
		if (pBooksTxtRecord)
		{
			return pBooksTxtRecord->nSpellIcon;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		D2_ASSERT(pItemsTxtRecord);
		return pItemsTxtRecord->nSpellIcon;
	}
}

//D2Common.0x6FD9D1E0 (#10805)
// 1.13c D2Common+0x22A20
uint8_t __stdcall ITEMS_GetDurWarnCount(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);

	return pItemsTxtRecord->nDurWarning;
}

//D2Common.0x6FD9D260 (#10806)
// 1.13c D2Common+0x229D0
uint8_t __stdcall ITEMS_GetQtyWarnCount(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);
	return pItemsTxtRecord->nQuantityWarning;
}

//D2Common.0x6FD9D2E0 (#10807)
// 1.13c D2Common+0x229A0
short __stdcall ITEMS_GetStrengthBonus(D2UnitStrc* pItem)
{
	D2ItemsTxt* pItemsTxtRecord = NULL;

	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		if (pItemsTxtRecord && pItemsTxtRecord->nStrBonus != 1)
		{
			return pItemsTxtRecord->nStrBonus;
		}
	}

	return 100;
}

//D2Common.0x6FD9D310 (#10808)
// 1.13c D2Common+0x22970
short __stdcall ITEMS_GetDexBonus(D2UnitStrc* pItem)
{
	D2ItemsTxt* pItemsTxtRecord = NULL;

	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		if (pItemsTxtRecord && pItemsTxtRecord->nDexBonus != 1)
		{
			return pItemsTxtRecord->nDexBonus;
		}
	}

	return 100;
}

//D2Common.0x6FD9D340 (#10809)
// 1.13c D2Common+0x22920
int __stdcall ITEMS_CheckIfSocketableByItemId(int nItemId)
{
	D2ItemsTxt* pItemsTxtRecord = NULL;

	pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);
	if (pItemsTxtRecord)
	{
		return pItemsTxtRecord->nHasInv;
	}

	return 0;
}

//D2Common.0x6FD9D360 (#10810)
// 1.13c D2Common+0x246F0
int __stdcall ITEMS_CheckIfSocketable(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		return ITEMS_CheckIfSocketableByItemId(pItem->dwClassId);
	}

	return 0;
}

//D2Common.0x6FD9D390 (#10811)
// 1.13c D2Common+0x26D40
BOOL __stdcall ITEMS_HasDurability(D2UnitStrc* pItem)
{
	D2ItemsTxt* pItemsTxtRecord = NULL;

	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		if (pItemsTxtRecord && !pItemsTxtRecord->nNoDurability && pItemsTxtRecord->nDurability && STATLIST_GetMaxDurabilityFromUnit(pItem))
		{
			return STATLIST_UnitGetStatValue(pItem, STAT_ITEM_INDESCTRUCTIBLE, 0) <= 0;
		}
	}

	return FALSE;
}

//D2Common.0x6FD9D3F0 (#10813)
// 1.13c D2Common+0x246A0
int __stdcall ITEMS_GetStaffMods(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		const int32_t nType = (int32_t)ITEMS_GetItemType(pItem);
		if (nType >= 0 && nType < sgptDataTables->nItemTypesTxtRecordCount)
		{
			return sgptDataTables->pItemTypesTxt[nType].nStaffMods;
		}
	}

	return 7;
}

//D2Common.0x6FD9D470 (#10814)
// 1.13c D2Common+0x228D0
uint8_t __stdcall ITEMS_GetAllowedGemSocketsFromItemId(int nItemId)
{
	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);

	if (pItemsTxtRecord)
	{
		return pItemsTxtRecord->nGemSockets;
	}

	return 0;
}

//D2Common.0x6FD9D490 (#10815)
// 1.13c D2Common+0x24610
uint8_t __stdcall ITEMS_GetMaxSockets(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		const int32_t nType = (int32_t)ITEMS_GetItemType(pItem);
		if (nType >= 0 && nType < sgptDataTables->nItemTypesTxtRecordCount)
		{
			const D2ItemTypesTxt* pType = &sgptDataTables->pItemTypesTxt[nType];
			const uint8_t nGemSockets = ITEMS_GetAllowedGemSocketsFromItemId(pItem->dwClassId);
			// 1.13c: item level via ITEMS_GetItemLevel (0x23810), which clamps it to at least 1
			const int nItemLevel = ITEMS_GetItemLevel(pItem);
			uint8_t nMaxSockets;
			if (nItemLevel < 26)
			{
				nMaxSockets = pType->nMaxSock1;
			}
			else if (nItemLevel < 41)
			{
				nMaxSockets = pType->nMaxSock25;
			}
			else
			{
				nMaxSockets = pType->nMaxSock40;
			}

			return nGemSockets <= nMaxSockets ? nGemSockets : nMaxSockets;
		}
	}

	return 0;
}

//D2Common.0x6FD9D580 (#10816)
// 1.13c D2Common+0x26080
int __stdcall ITEMS_GetSockets(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	return STATLIST_UnitGetStatValue(pItem, STAT_ITEM_NUMSOCKETS, 0);
}

//D2Common.0x6FD9D5E0 (#10817)
// 1.13c D2Common+0x2A480
void __stdcall ITEMS_AddSockets(D2UnitStrc* pItem, int nSockets)
{
	D2ItemsTxt* pItemsTxtRecord = NULL;
	uint32_t nQuality = 0;
	int nCurrentSockets = 0;
	int nMaxSockets = 0;
	uint8_t nHeight = 0;
	uint8_t nWidth = 0;

	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		nMaxSockets = 6;
		nWidth = 0;
		nHeight = 0;

		pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		if (pItemsTxtRecord)
		{
			nWidth = pItemsTxtRecord->nInvWidth;
			nHeight = pItemsTxtRecord->nInvHeight;
		}

		if (nWidth * nHeight <= 6)
		{
			nMaxSockets = nWidth * nHeight;
		}

		if (nWidth * nHeight)
		{
			if (pItem->pItemData)
			{
				nQuality = pItem->pItemData->dwQualityNo;
			}
			else
			{
				nQuality = ITEMQUAL_NORMAL;
			}

			switch (nQuality)
			{
			case ITEMQUAL_SET:
			case ITEMQUAL_UNIQUE:
				nCurrentSockets = STATLIST_UnitGetStatValue(pItem, STAT_ITEM_NUMSOCKETS, 0);
				if (nCurrentSockets <= 0)
				{
					if (nMaxSockets >= 1)
					{
						nMaxSockets = 1;
					}
				}
				else
				{
					nSockets = nCurrentSockets;
				}
				break;
			case ITEMQUAL_RARE:
				if (nMaxSockets >= 2)
				{
					nMaxSockets = 2;
				}
				break;
			case ITEMQUAL_MAGIC:
				if (nMaxSockets >= 4)
				{
					nMaxSockets = 4;
				}
				break;
			case ITEMQUAL_CRAFT:
			case ITEMQUAL_TEMPERED:
				if (nMaxSockets >= 3)
				{
					nMaxSockets = 3;
				}
				break;
			default:
				break;
			}

			// 1.13c: the per-type and per-level cap comes from ITEMS_GetMaxSockets
			const int nAllowedSockets = ITEMS_GetMaxSockets(pItem);
			if (nMaxSockets >= nAllowedSockets)
			{
				nMaxSockets = nAllowedSockets;
			}

			if (nSockets <= 1)
			{
				nSockets = 1;
			}

			if (nSockets >= nMaxSockets)
			{
				nSockets = nMaxSockets;
			}

			if (nSockets > 0)
			{
				if (pItem->pItemData)
				{
					pItem->pItemData->dwItemFlags |= IFLAG_SOCKETED;
				}

				STATLIST_SetUnitStat(pItem, STAT_ITEM_NUMSOCKETS, nSockets, 0);
			}
		}
	}
}

//D2Common.0x6FD9D7C0 (#10818)
// 1.13c D2Common+0x2A3D0
void __stdcall ITEMS_SetSockets(D2UnitStrc* pItem, int nSockets)
{
	D2ItemsTxt* pItemsTxtRecord = NULL;
	int nMaxSockets = 0;
	uint8_t nWidth = 0;
	uint8_t nHeight = 0;

	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		nMaxSockets = 6;
		nWidth = 0;
		nHeight = 0;

		pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		if (pItemsTxtRecord)
		{
			nWidth = pItemsTxtRecord->nInvWidth;
			nHeight = pItemsTxtRecord->nInvHeight;
		}

		if (nWidth * nHeight <= 6)
		{
			nMaxSockets = nWidth * nHeight;
		}

		// 1.13c: any nonzero size works (1.10f only ran for a size above 6); the cap comes from ITEMS_GetMaxSockets
		if (nWidth * nHeight)
		{
			const int nAllowedSockets = ITEMS_GetMaxSockets(pItem);
			if (nMaxSockets >= nAllowedSockets)
			{
				nMaxSockets = nAllowedSockets;
			}

			if (nSockets <= 1)
			{
				nSockets = 1;
			}
			if (nSockets >= nMaxSockets)
			{
				nSockets = nMaxSockets;
			}

			if (pItem->pItemData)
			{
				pItem->pItemData->dwItemFlags |= IFLAG_SOCKETED;
			}

			STATLIST_SetUnitStat(pItem, STAT_ITEM_NUMSOCKETS, nSockets, 0);
		}
	}
}

//D2Common.0x6FD9D900 (#10819)
// 1.13c D2Common+0x22880
int __stdcall ITEMS_GetGemApplyTypeFromItemId(int nItemId)
{
	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);

	D2_ASSERT(pItemsTxtRecord);

	return pItemsTxtRecord->nGemApplyType;
}

//D2Common.0x6FD9D940 (#10820)
// 1.13c D2Common+0x245D0
int __stdcall ITEMS_GetGemApplyType(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	return ITEMS_GetGemApplyTypeFromItemId(pItem->dwClassId);
}

//D2Common.0x6FD9D9D0 (#10821)
// 1.13c D2Common+0x245C0
BOOL __stdcall ITEMS_IsSocketFiller(D2UnitStrc* pItem)
{
	return ITEMS_CheckItemTypeId(pItem, ITEMTYPE_SOCKET_FILLER);
}

//D2Common.0x6FD9D9E0 (#10822)
// 1.13c D2Common+0x26ba0
const D2RunesTxt* __stdcall ITEMS_GetRunesTxtRecordFromItem(const D2UnitStrc* pItem)
{
	if (!pItem)
	{
		return nullptr;
	}

	const bool isItemOfSpecialQuality = (pItem->dwUnitType == UNIT_ITEM && pItem->pItemData && pItem->pItemData->dwQualityNo >= ITEMQUAL_MAGIC && pItem->pItemData->dwQualityNo <= ITEMQUAL_TEMPERED);
	// Note: The game specifically checks all the things above, which means we will enter the condition if dwUnitType == UNIT_ITEM
	// While this is not really correct, this is what the game does, and it works because it expects the unit to be an item.
	// and is using another function that checked if an item was special, which happened to check that the unit was indeed an item
	if (isItemOfSpecialQuality)
	{
		return nullptr;
	}

	// 1.13c delta: retail also requires the socketed inventory's signature (0x1020304) to be intact.
	if (!DATATBLS_GetItemsTxtRecord(pItem->dwClassId)->nQuest && pItem->pInventory && pItem->pInventory->dwSignature == D2C_InventoryHeader)
	{
		// 1.13c D2Common+0x26BA0: a socketed inventory with no first item returns 0 before the socket walk.
		D2UnitStrc* pFirstInventoryItem = INVENTORY_GetFirstItem(pItem->pInventory);
		if (!pFirstInventoryItem)
		{
			return nullptr;
		}

		int nSocketableIds[6] = {};
		int nSocketedItems = 0;

		for (D2UnitStrc* pInventoryItem = pFirstInventoryItem; pInventoryItem; pInventoryItem = INVENTORY_GetNextItem(pInventoryItem))
		{
			if (!INVENTORY_UnitIsItem(pInventoryItem))
			{
				return nullptr; // 1.13c delta: non-item in inventory chain aborts
			}
			nSocketableIds[nSocketedItems] = pInventoryItem->dwClassId;
			++nSocketedItems;
		}

		D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

		// retail D2Common 0x26BA0: the socket count is compared through a byte mask (retail `& 0xff`)
		const int nSockets = STATLIST_UnitGetStatValue(pItem, STAT_ITEM_NUMSOCKETS, 0) & 0xFF;

		if (nSockets != nSocketedItems)
		{
			return nullptr;
		}

		for (int i = 0; i < sgptDataTables->pRuneDataTables.nRunesTxtRecordCount; ++i)
		{
			const D2RunesTxt* pRunesTxtRecord = &sgptDataTables->pRuneDataTables.pRunesTxt[i];

			if (pRunesTxtRecord->nComplete)
			{
				int nRuneCounter = 0;
				bool bMatch = true;

				while (nRuneCounter < 6 && pRunesTxtRecord->nRune[nRuneCounter] > 0)
				{
					if (nSocketedItems < nRuneCounter || nSocketableIds[nRuneCounter] != pRunesTxtRecord->nRune[nRuneCounter])
					{
						bMatch = false;
						break;
					}

					++nRuneCounter;
				}

				if (bMatch && nSockets <= nRuneCounter) // 1.13c delta: recipe must cover all sockets
				{
					// All runes are a match, now check if the item types correspond
					bool isItemTypeExcluded = false;
					for (int nETypeCounter = 0; nETypeCounter < 3 && pRunesTxtRecord->wEType[nETypeCounter]; ++nETypeCounter)
					{
						if (ITEMS_CheckItemTypeId(pItem, pRunesTxtRecord->wEType[nETypeCounter]))
						{
							isItemTypeExcluded = true;
							break;
						}
					}

					// Not an excluded type, check valid types
					if (!isItemTypeExcluded)
					{
						for (int nITypeCounter = 0; nITypeCounter < 6 && pRunesTxtRecord->wIType[nITypeCounter]; ++nITypeCounter)
						{
							if (ITEMS_CheckItemTypeId(pItem, pRunesTxtRecord->wIType[nITypeCounter]))
							{
								return pRunesTxtRecord;
							}
						}
					}
				}
			}
		}
	}

	return nullptr;
}


// not-1.13c: retail inlines this bit test into its type-equivalence checks (D2Common+0x24500, 0x24430)
static BOOL ITEMS_CheckTypeEquivalenceFromLUT(uint32_t* pEquivalenceLUT, uint32_t nItemType)
{
	return pEquivalenceLUT[nItemType / 32] & gdwBitMasks[nItemType % 32];
}

// not-1.13c: retail inlines the row address (LUT base + type * nItemTypesIndex words) in its callers 0x24430 and 0x24500
static uint32_t* ITEMS_TypeEquivalenceLUT(int nItemTypeLUT)
{
	return &sgptDataTables->pItemTypesEquivalenceLUTs[nItemTypeLUT * sgptDataTables->nItemTypesIndex];
}

//D2Common.0x6FD9DBA0 (#10729)
// 1.13c D2Common+0x24500: the type-equivalence walk is the vendored one step for step -- the
// primary wType[0] lookup, then wType[1] only when the primary misses and is in range.
BOOL __stdcall ITEMS_CheckItemTypeIdByItemId(int nItemId, int nItemType)
{
	if (nItemType >= 0 && nItemType < sgptDataTables->nItemTypesTxtRecordCount)
	{
		const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);
		if (!pItemsTxtRecord)
		{
			// 1.13c D2Common+0x24500: an item id past the items table answers 0 (no assert).
			return FALSE;
		}
		if (pItemsTxtRecord->wType[0] >= 0 && pItemsTxtRecord->wType[0] < sgptDataTables->nItemTypesTxtRecordCount)
		{
			if (ITEMS_CheckTypeEquivalenceFromLUT(ITEMS_TypeEquivalenceLUT(pItemsTxtRecord->wType[0]), nItemType))
			{
				return TRUE;
			}

			if (pItemsTxtRecord->wType[1] > 0 && pItemsTxtRecord->wType[1] < sgptDataTables->nItemTypesTxtRecordCount)
			{
				return ITEMS_CheckTypeEquivalenceFromLUT(ITEMS_TypeEquivalenceLUT(pItemsTxtRecord->wType[1]), nItemType);
			}
		}
	}

	return FALSE;
}

//D2Common.0x6FD9DC80 (#10730)
// 1.13c D2Common+0x22820
BOOL __stdcall ITEMS_CheckType(int nItemType1, int nItemType2)
{
	if (nItemType1 >= 0 && nItemType1 < sgptDataTables->nItemTypesTxtRecordCount && nItemType2 >= 0 && nItemType2 < sgptDataTables->nItemTypesTxtRecordCount)
	{
		return ITEMS_CheckTypeEquivalenceFromLUT(ITEMS_TypeEquivalenceLUT(nItemType1), nItemType2);
	}

	return FALSE;
}

//D2Common.0x6FD9DCE0 (#10731)
// 1.13c D2Common+0x24430: retail fuses this null guard with ITEMS_CheckItemTypeIdByItemId's body;
// the class id reaches items.txt through DATATBLS_GetItemTypeFromItemId (same result, see the spec).
BOOL __stdcall ITEMS_CheckItemTypeId(const D2UnitStrc* pItem, int nItemType)
{
	if (pItem)
	{
		return ITEMS_CheckItemTypeIdByItemId(pItem->dwClassId, nItemType);
	}

	return FALSE;
}

//D2Common.0x6FD9DDD0 (#10803)
//TODO: Find a better name
// 1.13c D2Common+0x227C0
int __stdcall ITEMS_CheckBitField1Flag1(int nItemId)
{
	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);

	D2_ASSERT(pItemsTxtRecord);

	return pItemsTxtRecord->dwBitField1 & 1;
}

//D2Common.0x6FD9DE10 (#10802)
// 1.13c D2Common+0x22760
int __stdcall ITEMS_IsMetalItem(int nItemId)
{
	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);

	D2_ASSERT(pItemsTxtRecord);

	return pItemsTxtRecord->dwBitField1 & 2;
}

//D2Common.0x6FD9DE50 (#10801) - Unused
// 1.13c D2Common+0x22700
int __stdcall ITEMS_CheckBitField1Flag4(int nItemId)
{
	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);

	D2_ASSERT(pItemsTxtRecord);

	return pItemsTxtRecord->dwBitField1 & 4;
}

//D2Common.0x6FD9DE90 (#10774)
// 1.13c D2Common+0x22690
BOOL __stdcall ITEMS_IsNotQuestItemByItemId(int nItemId)
{
	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);

	D2_ASSERT(pItemsTxtRecord);

	return pItemsTxtRecord->nQuest < 1 && pItemsTxtRecord->wType[0] != ITEMTYPE_QUEST;
}

// 1.13c D2Common+0x22640
// Retail checks unit, item type and data separately before reading data +0x28.
//D2Common.0x6FD9DEE0 (#10732)
int __stdcall ITEMS_GetFileIndex(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);
	D2_ASSERT(pItem->pItemData);
	return pItem->pItemData->dwFileIndex;
}

//D2Common.0x6FD9DF60 (#10733)
// 1.13c D2Common+0x225F0
void __stdcall ITEMS_SetFileIndex(D2UnitStrc* pItem, uint32_t dwFileIndex)
{
	D2_ASSERT(ITEMS_GetItemData(pItem));
	pItem->pItemData->dwFileIndex = dwFileIndex;
}

//D2Common.0x6FD9DFE0 (#11244)
// 1.13c D2Common+0x22590
void __stdcall ITEMS_GetRealmData(D2UnitStrc* pItem, int* pRealmData0, int* pRealmData1)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		*pRealmData0 = pItemData->dwRealmData[0];
		*pRealmData1 = pItemData->dwRealmData[1];
	}
	else
	{
		*pRealmData0 = 0;
		*pRealmData1 = 0;
	}
}

// 1.13c D2Common+0x22560 (#11063, alias 0x5E070 #11245)
void __stdcall ITEMS_SetRealmData(D2UnitStrc* pItem, int a2, int a3)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		pItemData->dwRealmData[0] = a2;
		pItemData->dwRealmData[1] = a3;
	}
}

//D2Common.0x6FD9E0A0 (#10734)
// 1.13c D2Common+0x22510
void __stdcall ITEMS_SetOwnerId(D2UnitStrc* pItem, D2UnitGUID nOwnerGUID)
{
	D2_ASSERT(ITEMS_GetItemData(pItem));
	pItem->pItemData->dwOwnerGUID = nOwnerGUID;
}

//D2Common.0x6FD9E120 (#10735)
// 1.13c D2Common+0x224C0
int __stdcall ITEMS_GetOwnerId(D2UnitStrc* pItem)
{
	D2_ASSERT(ITEMS_GetItemData(pItem));
	return pItem->pItemData->dwOwnerGUID;
}

//D2Common.0x6FD9E1A0 (#10736)
// 1.13c D2Common+0x243B0: body locations via ITEMS_GetAllowedBodyLocations, the type via ITEMS_GetItemType
BOOL __stdcall ITEMS_IsBodyItem(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		uint8_t nBodyLoc1 = 0;
		uint8_t nBodyLoc2 = 0;
		ITEMS_GetAllowedBodyLocations(pItem, &nBodyLoc1, &nBodyLoc2);
		if (nBodyLoc1 || nBodyLoc2)
		{
			const int32_t nItemType = (int32_t)ITEMS_GetItemType(pItem);
			if (nItemType >= 0 && nItemType < sgptDataTables->nItemTypesTxtRecordCount && sgptDataTables->pItemTypesTxt)
			{
				return sgptDataTables->pItemTypesTxt[nItemType].nBody;
			}
		}
	}

	return FALSE;
}

//D2Common.0x6FD9E2A0 (#10738)
// 1.13c D2Common+0x24320
BOOL __stdcall ITEMS_IsClassValidByItemId(int nItemId)
{
	D2ItemTypesTxt* pItemTypesTxtRecord = NULL;
	D2ItemsTxt* pItemsTxtRecord = NULL;

	pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);
	D2_ASSERT(pItemsTxtRecord);

	if (pItemsTxtRecord->wType[0] >= 0 && pItemsTxtRecord->wType[0] < sgptDataTables->nItemTypesTxtRecordCount)
	{
		pItemTypesTxtRecord = &sgptDataTables->pItemTypesTxt[pItemsTxtRecord->wType[0]];
		if (pItemTypesTxtRecord)
		{
			return pItemTypesTxtRecord->nClass < 7;
		}
	}

	return FALSE;
}

//D2Common.0x6FD9E310 (#10737)
// 1.13c D2Common+0x242D0: the type comes from ITEMS_GetItemType, not the items row
BOOL __stdcall ITEMS_IsClassValid(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		const int32_t nItemType = (int32_t)ITEMS_GetItemType(pItem);
		if (nItemType >= 0 && nItemType < sgptDataTables->nItemTypesTxtRecordCount && sgptDataTables->pItemTypesTxt)
		{
			return sgptDataTables->pItemTypesTxt[nItemType].nClass < 7;
		}
	}

	return FALSE;
}

//D2Common.0x6FD9E390 (#10739)
// 1.13c D2Common+0x24280: the type comes from ITEMS_GetItemType, not the items row
int __stdcall ITEMS_GetClassOfClassSpecificItem(D2UnitStrc* pItem)
{
	if (pItem && pItem->dwUnitType == UNIT_ITEM)
	{
		const int32_t nItemType = (int32_t)ITEMS_GetItemType(pItem);
		if (nItemType >= 0 && nItemType < sgptDataTables->nItemTypesTxtRecordCount && sgptDataTables->pItemTypesTxt)
		{
			const uint8_t nClass = sgptDataTables->pItemTypesTxt[nItemType].nClass;
			if (nClass < NUMBER_OF_PLAYERCLASSES)
			{
				return nClass;
			}
		}
	}

	return PCLASS_INVALID;
}

//D2Common.0x6FD9E410 (#10823)
// 1.13c D2Common+0x22430
uint16_t __stdcall ITEMS_GetWeaponClassId(D2UnitStrc* pItem)
{
	static const D2CompositStrc stru_6FDE3C08[8] =
	{
		{ ' wob', WEAPONCLASS_BOW },
		{ ' sh1', WEAPONCLASS_1HS },
		{ ' th1', WEAPONCLASS_1HT },
		{ ' fts', WEAPONCLASS_STF },
		{ ' sh2', WEAPONCLASS_2HS },
		{ ' th2', WEAPONCLASS_2HT },
		{ ' wbx', WEAPONCLASS_XBW },
		{ ' 1th', WEAPONCLASS_HT1 },
	};

	D2ItemsTxt* pItemsTxtRecord = NULL;
	int nCounter = 0;

	pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem ? pItem->dwClassId : -1);
	D2_ASSERT(pItemsTxtRecord);

	while (pItemsTxtRecord->dwWeapClass != stru_6FDE3C08[nCounter].nWeaponClassCode)
	{
		++nCounter;

		if (nCounter >= ARRAY_SIZE(stru_6FDE3C08))
		{
			return WEAPONCLASS_HTH;
		}
	}

	return (uint16_t)stru_6FDE3C08[nCounter].nWeaponClassId;
}

//D2Common.0x6FD9E480 (#10824)
// 1.13c D2Common+0x223D0
uint32_t __stdcall ITEMS_GetTransmogrifyFromItemId(int nItemId)
{
	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(nItemId);

	D2_ASSERT(pItemsTxtRecord);

	return pItemsTxtRecord->nTransmogrify != 0;
}

//D2Common.0x6FD9E4C0 (#10825)
// 1.13c D2Common+0x24240
uint32_t __stdcall ITEMS_GetTransmogrify(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	return ITEMS_GetTransmogrifyFromItemId(pItem->dwClassId);
}

//D2Common.0x6FD9E550 (#10826)
// 1.13c D2Common+0x24210
int __stdcall ITEMS_IsMagSetRarUniCrfOrTmp(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc* pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->dwQualityNo >= ITEMQUAL_MAGIC && pItemData->dwQualityNo <= ITEMQUAL_TEMPERED;
	}
	return false;
}

//D2Common.0x6FD9E580 (#10740)
// 1.13c D2Common+0x241D0
BOOL __stdcall ITEMS_IsNotQuestItem(D2UnitStrc* pItem)
{
	int nItemId = 0;

	if (pItem)
	{
		if (pItem->dwUnitType == UNIT_ITEM && pItem->pItemData && pItem->pItemData->dwItemFlags & IFLAG_NOSELL)
		{
			return FALSE;
		}

		nItemId = pItem->dwClassId;
	}
	else
	{
		nItemId = -1;
	}

	return ITEMS_IsNotQuestItemByItemId(nItemId);
}

//D2Common.0x6FD9E5F0 (#10827)
// 1.13c D2Common+0x22380
uint8_t __stdcall ITEMS_GetHitClassFromItem(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);

	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);
	return pItemsTxtRecord->nHitClass;
}

// 1.13c D2Common+0x22310
//D2Common.0x6FD9E670 (#10828)
int __stdcall ITEMS_Is1Or2Handed(D2UnitStrc* pPlayer, D2UnitStrc* pItem)
{
	D2ItemsTxt* pItemsTxtRecord = NULL;

	D2_ASSERT(pItem);
	D2_ASSERT(pPlayer);

	if (pPlayer->dwUnitType || pPlayer->dwClassId != PCLASS_BARBARIAN)
	{
		return FALSE;
	}

	pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);

	if (!pItemsTxtRecord->n1or2Handed)
	{
		return FALSE;
	}

	return TRUE;
}

//D2Common.0x6FD9E710 (#10829)
// 1.13c D2Common+0x251C0
uint8_t* __stdcall ITEMS_GetColor(D2UnitStrc* pPlayer, D2UnitStrc* pItem, uint8_t* pColor, int nTransType)
{
	D2UniqueItemsTxt* pUniqueItemsTxtRecord = NULL;
	D2MagicAffixTxt* pMagicAffixTxtRecord = NULL;
	D2MagicAffixTxt* pAutoAffixTxtRecord = NULL;
	D2SetItemsTxt* pSetItemsTxtRecord = NULL;
	D2ItemsTxt* pGemItemsTxtRecord = NULL;
	D2ItemsTxt* pItemsTxtRecord = NULL;
	D2StatesTxt* pStatesTxtRecord = NULL;
	D2GemsTxt* pGemsTxtRecord = NULL;
	D2UnitStrc* pInventoryItem = NULL;
	int nItemQuality = 0;
	int nGemSockets = 0;
	int nMaxSockets = 0;
	int nCounter = 0;
	uint16_t nAutoAffix = 0;

	if (!pItem)
	{
		return NULL;
	}

	// The 1.13c body (this row) gates the colour-state path on the first argument being a player,
	// monster or missile unit; objects and other unit types never reach the states loop.
	if (pPlayer && (pPlayer->dwUnitType == UNIT_PLAYER || pPlayer->dwUnitType == UNIT_MONSTER || pPlayer->dwUnitType == UNIT_MISSILE))
	{
		for (int i = 0; i < sgptDataTables->nColourStates; ++i)
		{
			if (STATES_CheckState(pPlayer, sgptDataTables->pColourStates[i]) && sgptDataTables->pColourStates[i] >= 0 && sgptDataTables->pColourStates[i] < sgptDataTables->nStatesTxtRecordCount)
			{
				pStatesTxtRecord = &sgptDataTables->pStatesTxt[sgptDataTables->pColourStates[i]];
				if (pStatesTxtRecord && pStatesTxtRecord->nItemTrans < 21 && ITEMS_CheckItemTypeId(pItem, pStatesTxtRecord->wItemType))
				{
					pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
					if (pItemsTxtRecord)
					{
						return D2CMP_MixPalette(pItemsTxtRecord->nTransform, pStatesTxtRecord->nItemTrans);
					}
				}
			}
		}
	}

	if (pItem->dwUnitType == UNIT_ITEM && pItem->pItemData)
	{
		nItemQuality = pItem->pItemData->dwQualityNo;
	}
	else
	{
		nItemQuality = ITEMQUAL_NORMAL;
	}

	switch (nItemQuality)
	{
	case ITEMQUAL_MAGIC:
	case ITEMQUAL_RARE:
		nCounter = 0;
		do
		{
			pMagicAffixTxtRecord = DATATBLS_GetMagicAffixTxtRecord(pItem->dwUnitType == UNIT_ITEM && pItem->pItemData ? pItem->pItemData->wMagicSuffix[nCounter] : 0);
			if (pMagicAffixTxtRecord && pMagicAffixTxtRecord->nTransformColor != -1)
			{
				pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
				if (pItemsTxtRecord->nTransform <= 0 || pItemsTxtRecord->nTransform >= 9 || pMagicAffixTxtRecord->nTransformColor >= 21)
				{
					*pColor = 0;
				}
				else
				{
					*pColor = (pMagicAffixTxtRecord->nTransformColor & 31) + 32 * pItemsTxtRecord->nTransform;
				}

				if (nTransType)
				{
					return D2CMP_MixPalette(pItemsTxtRecord->nInvTrans, pMagicAffixTxtRecord->nTransformColor);
				}
				else
				{
					return D2CMP_MixPalette(pItemsTxtRecord->nTransform, pMagicAffixTxtRecord->nTransformColor);
				}
			}

			++nCounter;
		}
		while (nCounter < ITEMS_MAX_MODS);

		nCounter = 0;
		do
		{
			pMagicAffixTxtRecord = DATATBLS_GetMagicAffixTxtRecord(pItem->dwUnitType == UNIT_ITEM && pItem->pItemData ? pItem->pItemData->wMagicPrefix[nCounter] : 0);
			if (pMagicAffixTxtRecord && pMagicAffixTxtRecord->nTransformColor != -1)
			{
				pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
				if (pItemsTxtRecord->nTransform <= 0 || pItemsTxtRecord->nTransform >= 9 || pMagicAffixTxtRecord->nTransformColor >= 21)
				{
					*pColor = 0;
				}
				else
				{
					*pColor = (pMagicAffixTxtRecord->nTransformColor & 31) + 32 * pItemsTxtRecord->nTransform;
				}

				if (nTransType)
				{
					return D2CMP_MixPalette(pItemsTxtRecord->nInvTrans, pMagicAffixTxtRecord->nTransformColor);
				}
				else
				{
					return D2CMP_MixPalette(pItemsTxtRecord->nTransform, pMagicAffixTxtRecord->nTransformColor);
				}
			}

			++nCounter;
		}
		while (nCounter < ITEMS_MAX_MODS);
		
		break;

	case ITEMQUAL_UNIQUE:
		pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		if (!pItemsTxtRecord)
		{
			return NULL;
		}

		D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);
		D2_ASSERT(pItem->pItemData);

		if (pItem->pItemData->dwFileIndex >= 0 && pItem->pItemData->dwFileIndex < sgptDataTables->nUniqueItemsTxtRecordCount)
		{
			pUniqueItemsTxtRecord = &sgptDataTables->pUniqueItemsTxt[pItem->pItemData->dwFileIndex];
			if (pUniqueItemsTxtRecord)
			{
				if (nTransType)
				{
					if (pUniqueItemsTxtRecord->nInvTransform >= 0)
					{
						if (pItemsTxtRecord->nTransform <= 0 || pItemsTxtRecord->nTransform >= 9 || pUniqueItemsTxtRecord->nChrTransform < 0 || pUniqueItemsTxtRecord->nChrTransform >= 21)
						{
							*pColor = 0;
						}
						else
						{
							*pColor = (pUniqueItemsTxtRecord->nChrTransform & 31) + 32 * pItemsTxtRecord->nTransform;
						}

						return D2CMP_MixPalette(pItemsTxtRecord->nInvTrans, pUniqueItemsTxtRecord->nInvTransform);
					}
				}
				else
				{
					if (pUniqueItemsTxtRecord->nChrTransform >= 0)
					{
						if (pItemsTxtRecord->nTransform <= 0 || pItemsTxtRecord->nTransform >= 9 || pUniqueItemsTxtRecord->nChrTransform < 0 || pUniqueItemsTxtRecord->nChrTransform >= 21)
						{
							*pColor = 0;
						}
						else
						{
							*pColor = (pUniqueItemsTxtRecord->nChrTransform & 31) + 32 * pItemsTxtRecord->nTransform;
						}

						return D2CMP_MixPalette(pItemsTxtRecord->nTransform, pUniqueItemsTxtRecord->nChrTransform);
					}
				}
			}
		}

		return NULL;

	case ITEMQUAL_SET:
		pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		if (!pItemsTxtRecord)
		{
			return NULL;
		}

		D2_ASSERT(pItem->dwUnitType == UNIT_ITEM);
		D2_ASSERT(pItem->pItemData);

		if (pItem->pItemData->dwFileIndex >= 0 && pItem->pItemData->dwFileIndex < sgptDataTables->nSetItemsTxtRecordCount)
		{
			pSetItemsTxtRecord = &sgptDataTables->pSetItemsTxt[pItem->pItemData->dwFileIndex];
			if (pSetItemsTxtRecord)
			{
				if (nTransType)
				{
					if (pSetItemsTxtRecord->nInvTransform >= 0)
					{
						if (pItemsTxtRecord->nTransform <= 0 || pItemsTxtRecord->nTransform >= 9 || pSetItemsTxtRecord->nChrTransform < 0 || pSetItemsTxtRecord->nChrTransform >= 21)
						{
							*pColor = 0;
						}
						else
						{
							*pColor = (pSetItemsTxtRecord->nChrTransform & 31) + 32 * pItemsTxtRecord->nTransform;
						}

						return D2CMP_MixPalette(pItemsTxtRecord->nInvTrans, pSetItemsTxtRecord->nInvTransform);
					}
				}
				else
				{
					if (pSetItemsTxtRecord->nChrTransform >= 0)
					{
						if (pItemsTxtRecord->nTransform <= 0 || pItemsTxtRecord->nTransform >= 9 || pSetItemsTxtRecord->nChrTransform < 0 || pSetItemsTxtRecord->nChrTransform >= 21)
						{
							*pColor = 0;
						}
						else
						{
							*pColor = (pSetItemsTxtRecord->nChrTransform & 31) + 32 * pItemsTxtRecord->nTransform;
						}

						return D2CMP_MixPalette(pItemsTxtRecord->nTransform, pSetItemsTxtRecord->nChrTransform);
					}
				}
			}
		}

		return NULL;

	default:
		// The 1.13c body (this row) tests the item id for socketability (0x22920) and the item's
		// socket capacity (0x24610), then the socketed flag and the inventory signature, and only then
		// takes the first inventory item when it is a gem. The 1.10f body computed the same socket
		// count inline from the item type record.
		if (pItem->dwUnitType == UNIT_ITEM && ITEMS_CheckIfSocketableByItemId(pItem->dwClassId) && ITEMS_GetMaxSockets(pItem) != 0)
		{
			if (pItem->pItemData && (pItem->pItemData->dwItemFlags & IFLAG_SOCKETED) && pItem->pInventory && INVENTORY_CheckSignature(pItem->pInventory))
			{
				pInventoryItem = INVENTORY_GetFirstItem(pItem->pInventory);
				if (pInventoryItem && INVENTORY_UnitIsItem(pInventoryItem) && ITEMS_CheckItemTypeId(pInventoryItem, ITEMTYPE_GEM))
				{
					pGemItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pInventoryItem->dwClassId);
					if (pGemItemsTxtRecord)
					{
						pGemsTxtRecord = DATATBLS_GetGemsTxtRecord(pGemItemsTxtRecord->dwGemOffset);
						pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);

						if (pItemsTxtRecord->nTransform <= 0 || pItemsTxtRecord->nTransform >= 9 || pGemsTxtRecord->nTransForm >= 21)
						{
							*pColor = 0;
						}
						else
						{
							*pColor = (pGemsTxtRecord->nTransForm & 31) + 32 * pItemsTxtRecord->nTransform;
						}

						if (nTransType)
						{
							return D2CMP_MixPalette(pItemsTxtRecord->nInvTrans, pGemsTxtRecord->nTransForm);
						}
						else
						{
							return D2CMP_MixPalette(pItemsTxtRecord->nTransform, pGemsTxtRecord->nTransForm);
						}
					}
				}
			}
		}

		break;
	}

	if (pItem->dwUnitType == UNIT_ITEM && pItem->pItemData)
	{
		nAutoAffix = pItem->pItemData->wAutoAffix;
	}

	pAutoAffixTxtRecord = DATATBLS_GetMagicAffixTxtRecord(nAutoAffix);
	if (pAutoAffixTxtRecord && pAutoAffixTxtRecord->nTransformColor != -1)
	{
		pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
		if (pItemsTxtRecord->nTransform <= 0 || pItemsTxtRecord->nTransform >= 9 || pAutoAffixTxtRecord->nTransformColor >= 21)
		{
			*pColor = 0;
		}
		else
		{
			*pColor = (pAutoAffixTxtRecord->nTransformColor & 31) + 32 * pItemsTxtRecord->nTransform;
		}

		if (nTransType)
		{
			return D2CMP_MixPalette(pItemsTxtRecord->nInvTrans, pAutoAffixTxtRecord->nTransformColor);
		}
		else
		{
			return D2CMP_MixPalette(pItemsTxtRecord->nTransform, pAutoAffixTxtRecord->nTransformColor);
		}
	}

	return NULL;
}

//D2Common.0x6FD9EE70
// 1.13c D2Common+0x1800 (record id in EAX)
D2SetItemsTxt* __fastcall ITEMS_GetSetItemsTxtRecord(int nRecordId)
{
	if (nRecordId >= 0 && nRecordId < sgptDataTables->nSetItemsTxtRecordCount)
	{
		return &sgptDataTables->pSetItemsTxt[nRecordId];
	}

	return NULL;
}

// 1.13c D2Common+0x241A0 (EAX = unit): TRUE when the unit's inventory has a first item and that first
// node is an item unit (only the first node is looked at).
BOOL ITEMS_HasFirstInventoryItem(D2UnitStrc* pUnit)   // external linkage: the 241A0 probe calls this body
{
	D2InventoryStrc* pInventory = pUnit->pInventory;
	if (pInventory && INVENTORY_CheckSignature(pInventory) && pInventory->pFirstItem && pInventory->pFirstItem->dwUnitType == UNIT_ITEM)
	{
		return TRUE;
	}
	return FALSE;
}

//D2Common.0x6FD9EEA0 (#10830)
// 1.13c D2Common+0x250B0: asserts a null item; magic and better qualities (4..9) are refused, where
// 1.10f required them
BOOL __stdcall ITEMS_IsImbueable(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);

	if (pItem->dwUnitType == UNIT_ITEM && ITEMS_GetItemTypeFromItemId(pItem->dwClassId) == ITEMTYPE_GOLD)
	{
		return FALSE;
	}

	if (pItem->dwUnitType == UNIT_ITEM && pItem->pItemData && pItem->pItemData->dwItemFlags & IFLAG_NOSELL)
	{
		return FALSE;
	}

	if (!ITEMS_CheckBitField1Flag1(pItem->dwClassId))
	{
		return FALSE;
	}

	if (!(pItem->dwFlagEx & UNITFLAGEX_ISEXPANSION) && ITEMS_CheckIfThrowable(pItem))
	{
		return FALSE;
	}

	if (DATATBLS_GetItemsTxtRecord(pItem->dwClassId)->nQuest && ITEMS_GetBaseCode(pItem) != ' gel')
	{
		return FALSE;
	}

	if (ITEMS_HasFirstInventoryItem(pItem))
	{
		return FALSE;
	}

	if (ITEMS_CheckItemFlag(pItem, IFLAG_SOCKETED, __LINE__, __FILE__))
	{
		return FALSE;
	}

	switch (ITEMS_GetItemQuality(pItem))
	{
	case ITEMQUAL_MAGIC:
	case ITEMQUAL_SET:
	case ITEMQUAL_RARE:
	case ITEMQUAL_UNIQUE:
	case ITEMQUAL_CRAFT:
	case ITEMQUAL_TEMPERED:
		return FALSE;
	default:
		return TRUE;
	}
}

// 1.13c D2Common+0x240d0 (#10796)
// 1.13c delta: no unit-type gate; non-items fall through to the items.txt lookup by dwClassId.
BOOL __stdcall ITEMS_IsPersonalizable(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);
	if (pItem->dwUnitType == UNIT_ITEM && DATATBLS_GetItemsTxtRecord(pItem->dwClassId)->wType[0] == ITEMTYPE_GOLD)
	{
		return FALSE;
	}

	if (pItem->dwUnitType == UNIT_ITEM && pItem->pItemData && (pItem->pItemData->dwItemFlags & IFLAG_NOSELL))
	{
		return FALSE;
	}

	const int32_t nType = pItem->dwUnitType == UNIT_ITEM ? DATATBLS_GetItemsTxtRecord(pItem->dwClassId)->wType[0] : 0;
	if (nType == ITEMTYPE_PLAYER_BODY_PART || nType == ITEMTYPE_BOW_QUIVER || nType == ITEMTYPE_CROSSBOW_QUIVER)
	{
		return FALSE;
	}

	if (pItem->dwUnitType == UNIT_ITEM && pItem->pItemData && (pItem->pItemData->dwItemFlags & (IFLAG_BROKEN | IFLAG_PERSONALIZED)))
	{
		return FALSE;
	}

	D2ItemsTxt* pTxt = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	return !pTxt->nCompactSave && pTxt->nNameable != 0;
}

//D2Common.0x6FD9F260 (#10831)
// 1.13c D2Common+0x26AD0
BOOL __stdcall ITEMS_IsSocketable(D2UnitStrc* pItem)
{
	D2_ASSERT(pItem);

	if (pItem->dwUnitType == UNIT_ITEM && ITEMS_GetItemTypeFromItemId(pItem->dwClassId) == ITEMTYPE_GOLD)
	{
		return FALSE;
	}

	if (pItem->dwUnitType == UNIT_ITEM && pItem->pItemData && pItem->pItemData->dwItemFlags & IFLAG_NOSELL)
	{
		return FALSE;
	}

	if (DATATBLS_GetItemsTxtRecord(pItem->dwClassId)->nQuest && ITEMS_GetBaseCode(pItem) != ' gel')
	{
		return FALSE;
	}

	if (pItem->dwUnitType == UNIT_ITEM && pItem->pItemData && pItem->pItemData->dwItemFlags & IFLAG_BROKEN)
	{
		return FALSE;
	}

	if (ITEMS_HasFirstInventoryItem(pItem))
	{
		return FALSE;
	}

	if (pItem->dwUnitType == UNIT_ITEM && pItem->pItemData && pItem->pItemData->dwItemFlags & IFLAG_SOCKETED)
	{
		return FALSE;
	}

	// a non-item gets 0 from ITEMS_GetMaxSockets
	if (!ITEMS_GetMaxSockets(pItem))
	{
		return FALSE;
	}

	// 1.13c tests only the low byte of the socket count
	return (uint8_t)ITEMS_GetSockets(pItem) == 0;
}

//D2Common.0x6FD9F490 (#10877)
// 1.13c D2Common+0x2A210
int __stdcall ITEMS_GetAllRepairCosts(D2GameStrc* pGame, D2UnitStrc* pUnit, int nNpcId, D2C_Difficulties nDifficulty, D2BitBufferStrc* pQuestFlags, void(__fastcall* pfCallback)(D2GameStrc*, D2UnitStrc*, D2UnitStrc*))
{
	// 1.13c D2Common+0x2A210: per body slot, a repairable durability item below its maximum, a
	// refillable stack below its total max stack, or an item with a charged skill below its maximum
	// adds one repair transaction.
	if (!pUnit || !pUnit->pInventory)
	{
		return 0;
	}

	int nRepairCosts = 0;
	for (int nBodyLoc = 0; nBodyLoc < NUM_BODYLOC; ++nBodyLoc)
	{
		D2UnitStrc* pItem = INVENTORY_GetItemFromBodyLoc(pUnit->pInventory, nBodyLoc);
		if (!pItem)
		{
			continue;
		}

		BOOL bCanBeRepaired = FALSE;
		if (ITEMS_IsRepairable(pItem) && ITEMS_HasDurability(pItem))
		{
			const int nMaxDurability = STATLIST_GetMaxDurabilityFromUnit(pItem);
			if (nMaxDurability && STATLIST_UnitGetStatValue(pItem, STAT_DURABILITY, 0) != nMaxDurability)
			{
				bCanBeRepaired = TRUE;
			}
		}

		if (ITEMS_IsRepairableStack(pItem))
		{
			if (STATLIST_UnitGetStatValue(pItem, STAT_QUANTITY, 0) < ITEMS_GetTotalMaxStack(pItem))
			{
				bCanBeRepaired = TRUE;
			}
		}

		BOOL bChargesFull = FALSE;
		if ((ITEMS_HasUsedCharges(pItem, &bChargesFull) && !bChargesFull) || bCanBeRepaired)
		{
			nRepairCosts += ITEMS_CalculateTransactionCost(pUnit, pItem, nDifficulty, pQuestFlags, nNpcId, D2C_TransactionTypes::TRANSACTIONTYPE_REPAIR);

			if (pfCallback)
			{
				pfCallback(pGame, pItem, pUnit);
			}
		}
	}

	return nRepairCosts;
}

// 1.13c D2Common+0x21F70 (quality in EAX): TRUE for a quality of 1..3 or above 9.
BOOL ITEMS_IsPlainQuality(int nQuality)
{
	if (nQuality == 0)
	{
		return FALSE;
	}
	return nQuality <= 3 || nQuality > 9;
}

//D2Common.0x6FD9F720 (#10833)
// 1.13c D2Common+0x26930: null items compare by class -1, quality defaults to 2, the file index read
// asserts item data; only the low byte of the socket counts is tested
BOOL __stdcall ITEMS_AreStackablesEqual(D2UnitStrc* pItem1, D2UnitStrc* pItem2)
{
	if ((pItem1 ? (int)pItem1->dwClassId : -1) != (pItem2 ? (int)pItem2->dwClassId : -1))
	{
		return FALSE;
	}

	const int nQuality1 = (pItem1 && pItem1->dwUnitType == UNIT_ITEM && pItem1->pItemData) ? (int)pItem1->pItemData->dwQualityNo : ITEMQUAL_NORMAL;
	const int nQuality2 = (pItem2 && pItem2->dwUnitType == UNIT_ITEM && pItem2->pItemData) ? (int)pItem2->pItemData->dwQualityNo : ITEMQUAL_NORMAL;
	if (nQuality1 != nQuality2 || ITEMS_GetFileIndex(pItem1) != ITEMS_GetFileIndex(pItem2) || !ITEMS_CheckIfStackable(pItem1))
	{
		return FALSE;
	}

	if (ITEMS_IsEthereal(pItem1) != ITEMS_IsEthereal(pItem2)
		|| !ITEMS_IsPlainQuality(ITEMS_GetItemQuality(pItem1))
		|| !ITEMS_IsPlainQuality(ITEMS_GetItemQuality(pItem2))
		|| STATLIST_UnitGetStatValue(pItem1, STAT_MINDAMAGE, 0) != STATLIST_UnitGetStatValue(pItem2, STAT_MINDAMAGE, 0)
		|| STATLIST_UnitGetStatValue(pItem1, STAT_MAXDAMAGE, 0) != STATLIST_UnitGetStatValue(pItem2, STAT_MAXDAMAGE, 0)
		|| STATLIST_UnitGetStatValue(pItem1, STAT_SECONDARY_MINDAMAGE, 0) != STATLIST_UnitGetStatValue(pItem2, STAT_SECONDARY_MINDAMAGE, 0)
		|| STATLIST_UnitGetStatValue(pItem1, STAT_SECONDARY_MAXDAMAGE, 0) != STATLIST_UnitGetStatValue(pItem2, STAT_SECONDARY_MAXDAMAGE, 0)
		|| STATLIST_UnitGetStatValue(pItem1, STAT_ITEM_THROW_MINDAMAGE, 0) != STATLIST_UnitGetStatValue(pItem2, STAT_ITEM_THROW_MINDAMAGE, 0)
		|| STATLIST_UnitGetStatValue(pItem1, STAT_ITEM_THROW_MAXDAMAGE, 0) != STATLIST_UnitGetStatValue(pItem2, STAT_ITEM_THROW_MAXDAMAGE, 0))
	{
		return FALSE;
	}

	if ((uint8_t)ITEMS_GetSockets(pItem1) || (uint8_t)ITEMS_GetSockets(pItem2))
	{
		return FALSE;
	}

	return TRUE;
}

//D2Common.0x6FD9FA70 (#10834)
// 1.13c D2Common+0x2A170: both stats are plain base-layer reads; the stack test is ITEMS_GetTotalMaxStack
BOOL __stdcall ITEMS_CanItemBeUsedForThrowSkill(D2UnitStrc* pItem)
{
	if (!pItem)
	{
		return FALSE;
	}

	if (STATLIST_UnitGetStatValue(pItem, STAT_QUANTITY, 0) > 0)
	{
		return TRUE;
	}

	if (STATLIST_UnitGetStatValue(pItem, STAT_ITEM_THROWABLE, 0))
	{
		STATLIST_SetUnitStat(pItem, STAT_QUANTITY, 0, 0);
		return TRUE;
	}

	return ITEMS_GetTotalMaxStack(pItem) <= 0;
}


//D2Common.0x6FD9FB50 (#10836)
// 1.13c D2Common+0x23F90
uint32_t __stdcall ITEMS_GetSetItemsMask(D2UnitStrc* pPlayer, D2UnitStrc* pSetItem, BOOL bDontIgnoreInputItem)
{
	if (!pPlayer || !pPlayer->pInventory || ITEMS_GetItemQuality(pSetItem) != ITEMQUAL_SET)
	{
		return 0;
	}

	int nFileIndex = pSetItem->pItemData->dwFileIndex;
	D2SetItemsTxt* pSetItemTxt = DATATBLS_GetSetItemsTxtRecord(nFileIndex);
	if(!pSetItemTxt || !DATATBLS_GetSetsTxtRecord(pSetItemTxt->nSetId))
	{
		return 0;
	}

	uint32_t nSetItemMask = 0;
	for (D2UnitStrc* pEquippedItem = INVENTORY_GetFirstItem(pPlayer->pInventory); pEquippedItem; pEquippedItem = INVENTORY_GetNextItem(pEquippedItem))
	{
		if (INVENTORY_GetItemNodePage(pEquippedItem) == NODEPAGE_EQUIP)
		{
			if (INVENTORY_UnitIsItem(pEquippedItem) && (bDontIgnoreInputItem || pEquippedItem != pSetItem))
			{
				D2ItemDataStrc* pEquippedItemData = pEquippedItem->pItemData;
				if (pEquippedItemData && pEquippedItemData->dwQualityNo == ITEMQUAL_SET && !(pEquippedItemData->dwItemFlags & IFLAG_NOEQUIP) && !(pEquippedItemData->dwItemFlags & IFLAG_BROKEN))
				{
					if (D2SetItemsTxt* pEquippedSetItemsTxtRecord = DATATBLS_GetSetItemsTxtRecord(pEquippedItemData->dwFileIndex))
					{
						if (pEquippedSetItemsTxtRecord->nSetId == pSetItemTxt->nSetId)
						{
							nSetItemMask |= 1 << LOBYTE(pEquippedSetItemsTxtRecord->nSetItems);
						}
					}
				}
			}
		}
	}
	return nSetItemMask;
}

//D2Common.0x6FD9FD80 (#10838)
// 1.13c D2Common+0x23F40 (#10218)
D2SetItemsTxt* __stdcall ITEMS_GetSetItemsTxtRecordFromItem(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		if (pItemData->dwQualityNo == ITEMQUAL_SET
			&& pItemData->dwFileIndex >= 0
			&& pItemData->dwFileIndex < sgptDataTables->nSetItemsTxtRecordCount)
		{
			return &sgptDataTables->pSetItemsTxt[pItemData->dwFileIndex];
		}
	}

	return nullptr;
}

//D2Common.0x6FD9FE20 (#10839)
// 1.13c D2Common+0x23EF0
BOOL __stdcall ITEMS_CanBeEquipped(D2UnitStrc* pItem)
{
	if (pItem)
	{
		if (pItem->dwUnitType != UNIT_ITEM || !pItem->pItemData)
		{
			return TRUE;
		}

		return (!(pItem->pItemData->dwItemFlags & IFLAG_BROKEN) && !(pItem->pItemData->dwItemFlags & IFLAG_NOEQUIP));
	}

	return FALSE;
}

//D2Common.0x6FD9FE70 (#10840)
// 1.13c D2Common+0x287D0
BOOL __stdcall ITEMS_IsCharmUsable(D2UnitStrc* pItem, D2UnitStrc* pPlayer)
{
	// D2Common 1.13c +0x287D0: broken/noequip check applies only to item units; non-items fall through to the unit-type test
	if (!pItem)
	{
		return FALSE;
	}

	if (pItem->dwUnitType == UNIT_ITEM && pItem->pItemData && (pItem->pItemData->dwItemFlags & (IFLAG_BROKEN | IFLAG_NOEQUIP)))
	{
		return FALSE;
	}

	if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_CHARM) && pItem->dwUnitType == UNIT_ITEM && pItem->pItemData && pItem->pItemData->nInvPage == INVPAGE_INVENTORY)
	{
		return ITEMS_CheckRequirements(pItem, pPlayer, FALSE, NULL, NULL, NULL) != 0;
	}

	return FALSE;
}

// 1.13c D2Common+0x23E30
//D2Common.0x6FD9FF00 (#10776)
int __stdcall ITEMS_GetNoOfUnidItems(D2UnitStrc* pUnit)
{
	D2UnitStrc* pItem = NULL;
	int nUnidItems = 0;
	int nNodePage = 0;

	// 1.13c: the walk stops at the first non-item node and returns the count so far.
	pItem = INVENTORY_GetFirstItem(pUnit->pInventory);
	if (pItem)
	{
		nUnidItems = 0;
		while (INVENTORY_UnitIsItem(pItem))
		{
			if (!pItem->pItemData || !(pItem->pItemData->dwItemFlags & IFLAG_IDENTIFIED))
			{
				nNodePage = INVENTORY_GetItemNodePage(pItem);
				if (nNodePage != 1)
				{
					if (nNodePage == 3)
					{
						++nUnidItems;
					}
				}
				else
				{
					if (pItem->pItemData && (pItem->pItemData->nInvPage == INVPAGE_CUBE || pItem->pItemData->nInvPage == INVPAGE_INVENTORY))
					{
						++nUnidItems;
					}
				}
			}

			pItem = INVENTORY_GetNextItem(pItem);
			if (!pItem)
			{
				return nUnidItems;
			}
		}
	}

	return nUnidItems;
}

// 1.13c D2Common+0x22260
//D2Common.0x6FD9FF90 (#10841)
int __stdcall ITEMS_GetBonusLifeBasedOnClass(D2UnitStrc* pPlayer, int nValue)
{
	if (!pPlayer || pPlayer->dwUnitType != UNIT_PLAYER)
	{
		return 2 * nValue;
	}
	else
	{
		switch (pPlayer->dwClassId)
		{
		case PCLASS_AMAZON:
		case PCLASS_PALADIN:
		case PCLASS_ASSASSIN:
			return nValue + (nValue >> 1);
		case PCLASS_BARBARIAN:
			return 2 * nValue;
		default:
			return nValue;
		}
	}
}

// 1.13c D2Common+0x22210
//D2Common.0x6FD9FFE0 (#10842)
int __stdcall ITEMS_GetBonusManaBasedOnClass(D2UnitStrc* pPlayer, int nValue)
{
	if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER)
	{
		switch (pPlayer->dwClassId)
		{
		case PCLASS_SORCERESS:
		case PCLASS_NECROMANCER:
		case PCLASS_DRUID:
			return 2 * nValue;
		case PCLASS_AMAZON:
		case PCLASS_PALADIN:
		case PCLASS_ASSASSIN:
			return nValue + (nValue >> 1);
		default:
			return nValue;
		}
	}

	return nValue;
}

// 1.13c D2Common+0x221C0
//D2Common.0x6FDA0030 (#10875)
uint16_t __stdcall ITEMS_GetItemFormat(D2UnitStrc* pItem)
{
	D2_ASSERT(ITEMS_GetItemData(pItem));
	return pItem->pItemData->wItemFormat;
}

// 1.13c D2Common+0x22170
//D2Common.0x6FDA00B0 (#10876)
void __stdcall ITEMS_SetItemFormat(D2UnitStrc* pItem, uint16_t nItemFormat)
{
	D2_ASSERT(ITEMS_GetItemData(pItem));
	pItem->pItemData->wItemFormat = nItemFormat;
}

// 1.13c D2Common+0x267F0 (the body; 1.13c always adds the item's faster-attack-rate stat)
//D2Common.0x6FDA0130 (#10878)
static int ITEMS_GetWeaponAttackSpeedImpl(D2UnitStrc* pUnit, D2UnitStrc* pWeapon, BOOL includeItemFasterAttackRate)
{
	D2ItemsTxt* pItemsTxtRecord = NULL;
	int nWeaponClass = 0;
	int v11 = 0;
	int v13 = 0;
	char szPath[8] = {};

	int nLength = 0;

	D2_ASSERT(pWeapon && pWeapon->dwUnitType == UNIT_ITEM);
	D2_ASSERT(ITEMS_CheckItemTypeId(pWeapon, ITEMTYPE_WEAPON));
	D2_ASSERT(pUnit);

	if (pWeapon->dwUnitType == UNIT_ITEM)
	{
		pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pWeapon->dwClassId);
		if (pItemsTxtRecord)
		{
			nWeaponClass = pItemsTxtRecord->dwWeapClass;
		}
	}

	D2Common_10884_COMPOSIT_unk(pUnit, pUnit->dwClassId, PLRMODE_ATTACK1, pUnit->dwUnitType, pUnit->pInventory, szPath, &nWeaponClass, 0, 0);

	if (DATATBLS_GetAnimDataInfo(szPath, &nLength, &v11, &v13))
	{
		D2_ASSERT(nLength);

		const int nFasterAttackRate = includeItemFasterAttackRate ? STATLIST_UnitGetItemStatOrSkillStatValue(pWeapon, STAT_ITEM_FASTERATTACKRATE, 0) : 0;
		return (nLength << 8) / (v11 * (STATLIST_UnitGetStatValue(pWeapon, STAT_ATTACKRATE, 0) + nFasterAttackRate + 100) / 100);
	}

	return 45;
}

// 1.13c D2Common+0x267F0
int __stdcall ITEMS_GetWeaponAttackSpeed(D2UnitStrc* pUnit, D2UnitStrc* pWeapon)
{
	return ITEMS_GetWeaponAttackSpeedImpl(pUnit, pWeapon, TRUE);
}

// not-1.13c: d2-web check-only variant (sole caller monspawn_check.cpp); D2Common has the one attack-speed function 0x267F0 (the TRUE variant above, sole caller 0x50CE0)
int __stdcall ITEMS_GetWeaponAttackSpeedForUnidentified(D2UnitStrc* pUnit, D2UnitStrc* pWeapon)
{
	return ITEMS_GetWeaponAttackSpeedImpl(pUnit, pWeapon, FALSE);
}

//D2Common.0x6FDA02B0 (#10879)
// 1.13c D2Common+0x24FF0
int __stdcall ITEMS_HasUsedCharges(D2UnitStrc* pItem, BOOL* pHasChargedSkills)
{
	D2StatStrc pStat[64] = {};
	int nCounter = 0;
	int nStats = 0;

	if (pHasChargedSkills)
	{
		*pHasChargedSkills = TRUE;
	}

	nStats = STATLIST_CopyStats(pItem, STAT_ITEM_CHARGED_SKILL, pStat, ARRAY_SIZE(pStat));

	if (nStats <= 0)
	{
		return FALSE;
	}
	else
	{
		if (pHasChargedSkills)
		{
			nCounter = 0;
			if (nStats > 0)
			{
				while ((uint8_t)pStat[nCounter].nValue >= (pStat[nCounter].nValue >> 8))
				{
					++nCounter;

					if (nCounter >= nStats)
					{
						return TRUE;
					}
				}

				*pHasChargedSkills = FALSE;
			}
		}

		return TRUE;
	}
}

//D2Common.0x6FDA0340 (#10880)
// 1.13c D2Common+0x23E00
BOOL __stdcall ITEMS_IsEthereal(D2UnitStrc* pItem)
{
	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		return pItemData->dwItemFlags & IFLAG_ETHEREAL;
	}

	return 0;
}

// 1.13c D2Common+0x22150: forwards to the versioned reader with the current save version 0x60.
//D2Common.0x6FDA0370 (#10883)
BOOL __stdcall ITEMS_GetCompactItemDataFromBitstream(uint8_t* pBitstream, size_t nSize, BOOL bCheckForHeader, D2ItemSaveStrc* pItemSave)
{
	return ITEMS_GetCompactItemDataFromBitstreamVersioned(pBitstream, nSize, bCheckForHeader, pItemSave, 0x60);
}

// 1.13c D2Common+0x22000 (ordinal 11171): ITEMS_GetCompactItemDataFromBitstream plus a trailing version word.
// Delta vs 1.10f: nVersion <= 0x5C turns the code 'nec ' into 'neg '; the code lookup is the items linker, no error log.
BOOL __stdcall ITEMS_GetCompactItemDataFromBitstreamVersioned(uint8_t* pBitstream, size_t nSize, BOOL bCheckForHeader, D2ItemSaveStrc* pItemSave, int32_t nVersion)
{
	D2BitBufferStrc pBuffer = {};
	BITMANIP_Initialize(&pBuffer, pBitstream, nSize);
	if (bCheckForHeader && BITMANIP_ReadSigned(&pBuffer, 16) != 'MJ')
	{
		return FALSE;
	}
	pItemSave->dwFlags = BITMANIP_Read(&pBuffer, 32);
	BITMANIP_Read(&pBuffer, 10);
	pItemSave->nAnimMode = (uint8_t)BITMANIP_Read(&pBuffer, 3);
	if (pItemSave->nAnimMode == IMODE_ONGROUND || pItemSave->nAnimMode == IMODE_DROPPING)
	{
		pItemSave->nX = (uint16_t)BITMANIP_Read(&pBuffer, 16);
		pItemSave->nY = (uint16_t)BITMANIP_Read(&pBuffer, 16);
	}
	else
	{
		pItemSave->nBodyloc = (uint8_t)BITMANIP_Read(&pBuffer, 4);
		pItemSave->nX = (uint16_t)BITMANIP_Read(&pBuffer, 4);
		pItemSave->nY = (uint16_t)BITMANIP_Read(&pBuffer, 4);
		pItemSave->nStorePage = (uint8_t)BITMANIP_Read(&pBuffer, 3) - 1;
	}
	int dwCode;
	if (pItemSave->dwFlags & IFLAG_ISEAR)
	{
		dwCode = ' rae';
	}
	else
	{
		dwCode = BITMANIP_Read(&pBuffer, 32);
		if (nVersion < 0x5D && dwCode == ' cen')
		{
			dwCode = ' gen';
		}
	}
	pItemSave->nClassId = FOG_GetLinkIndex(sgptDataTables->pItemsLinker, dwCode, 0);
	pItemSave->nItemFileIndex = (pItemSave->dwFlags & (IFLAG_LOWQUALITY | IFLAG_COMPACTSAVE)) ? 0 : BITMANIP_Read(&pBuffer, 3);
	return TRUE;
}

// 1.13c D2Common+0x2C470
//D2Common.0x6FDA0490 (#10882)
size_t __stdcall ITEMS_DecodeItemFromBitstream(D2UnitStrc* pItem, uint8_t* pBitstream, size_t nSize, BOOL bCheckForHeader, int* pSocketedItemCount, uint32_t dwVersion, BOOL* pFail)
{
	D2BitBufferStrc pBuffer = {};
	uint32_t dwFlags = 0;
	BOOL bGamble = FALSE;

	BITMANIP_Initialize(&pBuffer, pBitstream, nSize);

	if (bCheckForHeader && BITMANIP_ReadSigned(&pBuffer, 16) != 'MJ')
	{
		*pFail = TRUE;
		return 0;
	}

	*pFail = FALSE;

	dwFlags = BITMANIP_Read(&pBuffer, 32);
	if (dwFlags & IFLAG_LOWQUALITY)
	{
		bGamble = TRUE;
		dwFlags &= ~IFLAG_LOWQUALITY;
	}

	if (D2ItemDataStrc * pItemData = ITEMS_GetItemData(pItem))
	{
		pItemData->dwItemFlags = dwFlags;

		pItemData->dwItemFlags &= ~IFLAG_INIT;

		pItemData->dwItemLevel = 1;

		for (int i = 0; i < ITEMS_MAX_MODS; ++i)
		{
			pItemData->wMagicPrefix[i] = 0;
			pItemData->wMagicSuffix[i] = 0;
		}

		pItemData->wRarePrefix = 0;
		pItemData->wRareSuffix = 0;
	}

	if (dwFlags & IFLAG_COMPACTSAVE)
	{
		if (pSocketedItemCount)
		{
			*pSocketedItemCount = 0;
		}

		if (ITEMS_DecodeItemBitstreamCompact(pItem, &pBuffer, bCheckForHeader, dwVersion) <= 0)
		{
			*pFail = TRUE;
		}
	}
	else
	{
		if (ITEMS_DecodeItemBitstreamComplete(pItem, &pBuffer, bCheckForHeader, bGamble, pSocketedItemCount, dwVersion) <= 0)
		{
			*pFail = TRUE;
		}
	}

	return BITMANIP_GetSize(&pBuffer);
}

//D2Common.0x6FDA0620
// 1.13c D2Common+0x2C0C0 (EAX = item, EDI = buffer in retail; the second code lookup goes through the
// items linker, the quest-difficulty stat record is asserted present before it is read)
int __fastcall ITEMS_DecodeItemBitstreamCompact(D2UnitStrc* pItem, D2BitBufferStrc* pBuffer, BOOL bCheckForHeader, uint32_t dwVersion)
{
	D2ItemStatCostTxt* pItemStatCostTxtRecord = NULL;
	D2ItemsTxt* pItemsTxtRecord = NULL;
	D2StatListStrc* pStatList = NULL;
	int nBits = 0;
	int nItemId = 0;
	int nValue = 0;
	uint32_t dwCode = 0;
	char szChar = 0;
	uint8_t nAnimMode = 0;

	D2ItemDataStrc* pItemData = ITEMS_GetItemData(pItem);
	D2_ASSERT(pItemData);

	pItemData->wItemFormat = (uint16_t)BITMANIP_Read(pBuffer, 10);

	nAnimMode = (uint8_t)BITMANIP_Read(pBuffer, 3);
	if (nAnimMode == IMODE_ONGROUND || nAnimMode == IMODE_DROPPING)
	{
		UNITS_SetXForStaticUnit(pItem, BITMANIP_Read(pBuffer, 16));
		UNITS_SetYForStaticUnit(pItem, BITMANIP_Read(pBuffer, 16));
	}
	else
	{
		pItemData->nBodyLoc = (uint8_t)BITMANIP_Read(pBuffer, 4);
		UNITS_SetXForStaticUnit(pItem, BITMANIP_Read(pBuffer, 4));
		UNITS_SetYForStaticUnit(pItem, BITMANIP_Read(pBuffer, 4));
		pItemData->nInvPage = (uint8_t)BITMANIP_Read(pBuffer, 3) - 1;
	}
	UNITS_ChangeAnimMode(pItem, nAnimMode);

	if (pItemData->dwItemFlags & IFLAG_ISEAR)
	{
		ITEMS_SetFileIndex(pItem, BITMANIP_Read(pBuffer, 3));
		pItemData->nEarLvl = (uint8_t)BITMANIP_Read(pBuffer, 7);

		int i = 0;
		do
		{
			szChar = (char)BITMANIP_Read(pBuffer, 7);
			pItemData->szPlayerName[i] = szChar;
			++i;
		}
		while (szChar);
	}
	else
	{
		dwCode = BITMANIP_Read(pBuffer, 32);
		pItem->dwClassId = FOG_GetLinkIndex(sgptDataTables->pItemsLinker, dwCode, 0);

		if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_GOLD))
		{
			if (BITMANIP_Read(pBuffer, 1))
			{
				nBits = 32;
			}
			else
			{
				nBits = 12;
			}
			STATLIST_SetUnitStat(pItem, STAT_GOLD, BITMANIP_Read(pBuffer, nBits), 0);
		}

		if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_SCROLL))
		{
			if (dwCode == ' cst')
			{
				pItemData->wMagicSuffix[0] = 0;
			}
			else if (dwCode == ' csi')
			{
				pItemData->wMagicSuffix[0] = 1;
			}
		}
	}

	if (dwVersion > 92)
	{
		// 1.13c: the item's own code is looked up in the items linker again (no items.txt row-count test)
		nItemId = FOG_GetLinkIndex(sgptDataTables->pItemsLinker, ITEMS_GetBaseCode(pItem), 0);
		if (nItemId < 0 || !sgptDataTables->pItemDataTables.pItemsTxt)
		{
			return -1;
		}
		pItemsTxtRecord = &sgptDataTables->pItemDataTables.pItemsTxt[nItemId];

		if (pItemsTxtRecord->nQuest && pItemsTxtRecord->nQuestDiffCheck)
		{
			D2_ASSERT(sgptDataTables->nItemStatCostTxtRecordCount > STAT_QUESTITEMDIFFICULTY && sgptDataTables->pItemStatCostTxt);
			pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_QUESTITEMDIFFICULTY);
			nValue = (BITMANIP_Read(pBuffer, pItemStatCostTxtRecord->nSaveBits) - pItemStatCostTxtRecord->dwSaveAdd) << pItemStatCostTxtRecord->nValShift;

			pStatList = STATLIST_GetStatListFromUnitStateOrFlag(pItem, STATE_NONE, 0x40);
			if (!pStatList)
			{
				pStatList = STATLIST_AllocStatList(pItem->pMemoryPool, 0x40, 0, UNIT_ITEM, pItem->dwUnitId);
				D2COMMON_10475_PostStatToStatList(pItem, pStatList, 1);
			}
			if (pStatList)
			{
				STATLIST_SetStat(pStatList, STAT_QUESTITEMDIFFICULTY, nValue, 0);
			}
		}
	}

	if (bCheckForHeader && dwVersion > 86 && BITMANIP_Read(pBuffer, 1))
	{
		pItemData->dwRealmData[0] = BITMANIP_Read(pBuffer, 32);
		pItemData->dwRealmData[1] = BITMANIP_Read(pBuffer, 32);

		if (dwVersion > 93)
		{
			BITMANIP_Read(pBuffer, 32);
		}
	}

	pItem->dwInitSeed = 0;
	SEED_InitLowSeed(&pItem->pSeed, 0);

	pItemData->dwItemLevel = 1;
	pItemData->dwQualityNo = ITEMQUAL_NORMAL;

	return 1;
}

// 1.13c D2Common+0x2A600
//D2Common.0x6FDA0A20
int __fastcall ITEMS_DecodeItemBitstreamComplete(D2UnitStrc* pItem, D2BitBufferStrc* pBuffer, BOOL bCheckForHeader, BOOL bGamble, int* pSocketedItems, uint32_t dwVersion)
{
	static const int gnItemSetStates[] =
	{
		STATE_ITEMSET1, STATE_ITEMSET2, STATE_ITEMSET3, STATE_ITEMSET4, STATE_ITEMSET5
	};

	D2ItemStatCostTxt* pItemStatCostTxtRecord = NULL;
	D2MagicAffixDataTbl* pMagicAffixDataTbl = NULL;
	D2SetItemsTxt* pSetItemsTxtRecord = NULL;
	D2SkillsTxt* pSkillsTxtRecord = NULL;
	D2ItemsTxt* pItemsTxtRecord = NULL;
	D2SetsTxt* pSetsTxtRecord = NULL;
	D2ItemDataStrc* pItemData = NULL;
	D2StatListStrc* pStatList = NULL;
	char* szName = NULL;
	int nMagicPrefixOffset = 0;
	int nAutoMagicOffset = 0;
	int nSocketedItems = 0;
	int nSetItemMask = 0;
	int nSetRecordId = 0;
	int nLastStatId = 0;
	int nMaxSockets = 0;
	int nStatLists = 0;
	int nAnimMode = 0;
	int nClassId = 0;
	int nSkillId = 0;
	int nStatId = 0;
	int nState = 0;
	int nValue = 0;
	int nBits = 0;
	int nFlag = 0;
	int nMax = 0;

	int v237, v238, v240, v241, v242, v243, v244, v245, v246, v247, v248, v249, v250, v251, v252; //TODO: Change names

	uint32_t dwCode = 0;
	char szChar = 0;
	BOOL bRuneword = FALSE;
	BOOL bError = FALSE;
	BOOL b109 = FALSE;

	pItemData = ITEMS_GetItemData(pItem);
	D2_ASSERT(pItemData);

	pItemData->wItemFormat = (uint16_t)BITMANIP_Read(pBuffer, 10);

	nAnimMode = BITMANIP_Read(pBuffer, 3);

	if (nAnimMode == IMODE_ONGROUND || nAnimMode == IMODE_DROPPING)
	{
		UNITS_SetXForStaticUnit(pItem, BITMANIP_Read(pBuffer, 16));
		UNITS_SetYForStaticUnit(pItem, BITMANIP_Read(pBuffer, 16));
	}
	else
	{
		pItemData->nBodyLoc = (uint8_t)BITMANIP_Read(pBuffer, 4);

		UNITS_SetXForStaticUnit(pItem, BITMANIP_Read(pBuffer, 4));
		UNITS_SetYForStaticUnit(pItem, BITMANIP_Read(pBuffer, 4));

		pItemData->nInvPage = (uint8_t)(BITMANIP_Read(pBuffer, 3) - 1);
	}

	UNITS_ChangeAnimMode(pItem, nAnimMode);

	dwCode = BITMANIP_Read(pBuffer, 32);

	nClassId = DATATBLS_GetItemIdFromItemCode(dwCode);
	if (nClassId >= 0)
	{
		pItem->dwClassId = nClassId;
	}

	if (bGamble)
	{
		pItemData->dwItemLevel = 1;
		pItemData->dwQualityNo = ITEMQUAL_INFERIOR;

		return 1;
	}

	nSocketedItems = BITMANIP_Read(pBuffer, 3);
	if (pSocketedItems)
	{
		*pSocketedItems = nSocketedItems;
	}

	if (bCheckForHeader)
	{
		pItem->dwInitSeed = BITMANIP_Read(pBuffer, 32);
		SEED_InitLowSeed(&pItem->pSeed, pItem->dwInitSeed);
	}

	pItemData->dwItemLevel = BITMANIP_Read(pBuffer, 7);
	if (pItemData->dwItemLevel < 1)
	{
		pItemData->dwItemLevel = 1;
	}

	pItemData->dwQualityNo = BITMANIP_Read(pBuffer, 4);

	if (BITMANIP_Read(pBuffer, 1))
	{
		pItemData->nInvGfxIdx = (uint8_t)BITMANIP_Read(pBuffer, 3);
	}

	pMagicAffixDataTbl = DATATBLS_GetMagicAffixDataTables();

	nMagicPrefixOffset = pMagicAffixDataTbl->pMagicPrefix - pMagicAffixDataTbl->pMagicAffixTxt;
	nAutoMagicOffset = pMagicAffixDataTbl->pAutoMagic - pMagicAffixDataTbl->pMagicAffixTxt;

	switch (dwVersion)
	{
	case 72:
	case 73:
	case 80:
	case 81:
	case 82:
		nMagicPrefixOffset -= 661;
		nAutoMagicOffset -= 1262;
		break;
	case 83:
	case 84:
		nMagicPrefixOffset -= 674;
		nAutoMagicOffset -= 1265;
		break;
	case 85:
	case 86:
	case 87:
		nMagicPrefixOffset -= 675;
		nAutoMagicOffset -= 1266;
		break;
	case 88:
		nMagicPrefixOffset -= 729;
		nAutoMagicOffset -= 1440;
		break;
	default:
		break;
	}

	if (BITMANIP_Read(pBuffer, 1))
	{
		pItemData->wAutoAffix = (uint16_t)BITMANIP_Read(pBuffer, 11);
		if (dwVersion > 88)
		{
			if (dwVersion <= 89)
			{
				++pItemData->wAutoAffix;
			}

			if (pItemData->wAutoAffix)
			{
				pItemData->wAutoAffix += nAutoMagicOffset;
			}
		}
		else
		{
			pItemData->wAutoAffix = 0;
		}
	}
	else
	{
		pItemData->wAutoAffix = 0;
	}

	switch (pItemData->dwQualityNo)
	{
	case ITEMQUAL_INFERIOR:
	case ITEMQUAL_SUPERIOR:
	{
		pItemData->dwFileIndex = BITMANIP_Read(pBuffer, 3);
		break;
	}
	case ITEMQUAL_NORMAL:
	{
		if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_CHARM))
		{
			if (bCheckForHeader || pItemData->dwItemFlags & IFLAG_IDENTIFIED)
			{
				if (BITMANIP_Read(pBuffer, 1))
				{
					pItemData->wMagicPrefix[0] = (uint16_t)BITMANIP_Read(pBuffer, 11);
					if (pItemData->wMagicPrefix[0])
					{
						pItemData->wMagicPrefix[0] += nMagicPrefixOffset;
					}
				}
				else
				{
					pItemData->wMagicSuffix[0] = (uint16_t)BITMANIP_Read(pBuffer, 11);
				}
			}
		}

		if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_BODY_PART) && !ITEMS_CheckItemTypeId(pItem, ITEMTYPE_PLAYER_BODY_PART))
		{
			pItemData->dwFileIndex = BITMANIP_Read(pBuffer, 10);
		}

		if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_SCROLL) || ITEMS_CheckItemTypeId(pItem, ITEMTYPE_BOOK))
		{
			pItemData->wMagicSuffix[0] = (uint16_t)BITMANIP_Read(pBuffer, 5);
		}

		break;
	}
	case ITEMQUAL_RARE:
	case ITEMQUAL_CRAFT:
	{
		if (bCheckForHeader || pItemData->dwItemFlags & IFLAG_IDENTIFIED)
		{
			pItemData->wRarePrefix = (uint16_t)BITMANIP_Read(pBuffer, 8);
			pItemData->wRareSuffix = (uint16_t)BITMANIP_Read(pBuffer, 8);
		}

		for (int i = 0; i < ITEMS_MAX_MODS; ++i)
		{
			if (BITMANIP_Read(pBuffer, 1))
			{
				pItemData->wMagicPrefix[i] = (uint16_t)BITMANIP_Read(pBuffer, 11);
				if (dwVersion <= 89)
				{
					++pItemData->wMagicPrefix[i];
				}

				if (pItemData->wMagicPrefix[i])
				{
					pItemData->wMagicPrefix[i] += nMagicPrefixOffset;
				}
			}
			else
			{
				pItemData->wMagicPrefix[i] = 0;
			}

			if (BITMANIP_Read(pBuffer, 1))
			{
				pItemData->wMagicSuffix[i] = (uint16_t)BITMANIP_Read(pBuffer, 11);
				if (dwVersion <= 89)
				{
					++pItemData->wMagicSuffix[i];
				}
			}
			else
			{
				pItemData->wMagicSuffix[i] = 0;
			}
		}

		break;
	}
	case ITEMQUAL_TEMPERED:
	{
		if (bCheckForHeader || pItemData->dwItemFlags & IFLAG_IDENTIFIED)
		{
			pItemData->wRarePrefix = (uint16_t)BITMANIP_Read(pBuffer, 8);
			pItemData->wRareSuffix = (uint16_t)BITMANIP_Read(pBuffer, 8);
		}

		break;
	}
	case ITEMQUAL_MAGIC:
	{
		if (bCheckForHeader || pItemData->dwItemFlags & IFLAG_IDENTIFIED)
		{
			pItemData->wMagicPrefix[0] = (uint16_t)BITMANIP_Read(pBuffer, 11);
			if (pItemData->wMagicPrefix[0])
			{
				pItemData->wMagicPrefix[0] += nMagicPrefixOffset;
			}

			pItemData->wMagicSuffix[0] = (uint16_t)BITMANIP_Read(pBuffer, 11);
		}

		break;
	}
	case ITEMQUAL_UNIQUE:
	{
		if (bCheckForHeader || pItemData->dwItemFlags & IFLAG_IDENTIFIED)
		{
			pItemData->dwFileIndex = BITMANIP_Read(pBuffer, 12);
			if (pItemData->dwFileIndex < 0 || pItemData->dwFileIndex >= sgptDataTables->nUniqueItemsTxtRecordCount)
			{
				pItemData->dwFileIndex = -1;
			}
		}

		break;
	}
	case ITEMQUAL_SET:
	{
		if (bCheckForHeader || pItemData->dwItemFlags & IFLAG_IDENTIFIED)
		{
			bError = TRUE;

			nSetRecordId = BITMANIP_Read(pBuffer, 12);
			if (dwVersion > 92)
			{
				if (nSetRecordId >= 0 && nSetRecordId < sgptDataTables->nSetItemsTxtRecordCount)
				{
					pSetItemsTxtRecord = &sgptDataTables->pSetItemsTxt[nSetRecordId];
					if (pSetItemsTxtRecord)
					{
						pItemData->dwFileIndex = (uint32_t)(int32_t)(int16_t)pSetItemsTxtRecord->wSetItemId; // retail sign-extends the 16-bit id
						bError = FALSE;
					}
				}
			}
			else
			{
				if (nSetRecordId >= 0 && nSetRecordId < sgptDataTables->nSetsTxtRecordCount)
				{
					pSetsTxtRecord = &sgptDataTables->pSetsTxt[nSetRecordId];
					if (pSetsTxtRecord)
					{
						for (int i = 0; i < pSetsTxtRecord->nSetItems; ++i)
						{
							D2SetItemsTxt* pSetItem = DATATBLS_GetSetItemOfSet(pSetsTxtRecord, i);
							if (pSetItem && pSetItem->szItemCode == dwCode)
							{
								pItemData->dwFileIndex = (uint32_t)(int32_t)(int16_t)pSetItem->wSetItemId; // retail sign-extends the 16-bit id
								bError = FALSE;
								break;
							}
						}
					}
				}
			}
		}
		break;
	}
	default:
	{
		bError = TRUE;
		break;
	}
	}

	if (pItemData->dwItemFlags & IFLAG_RUNEWORD)
	{
		bRuneword = TRUE;
		pItemData->wMagicPrefix[0] = (uint16_t)BITMANIP_Read(pBuffer, 16);
	}

	if (pItemData->dwItemFlags & IFLAG_ISEAR)
	{
		pItemData->dwFileIndex = BITMANIP_Read(pBuffer, 3);
		pItemData->nEarLvl = (uint8_t)BITMANIP_Read(pBuffer, 7);

		szName = pItemData->szPlayerName;
		do
		{
			szChar = (char)BITMANIP_Read(pBuffer, 7);
			*szName++ = szChar;
		}
		while (szChar);
	}
	else if (pItemData->dwItemFlags & IFLAG_PERSONALIZED)
	{
		szName = pItemData->szPlayerName;
		do
		{
			szChar = (char)BITMANIP_Read(pBuffer, 7);
			*szName++ = szChar;
		}
		while (szChar);
	}

	if (bCheckForHeader && dwVersion > 86 && BITMANIP_Read(pBuffer, 1))
	{
		pItemData->dwRealmData[0] = BITMANIP_Read(pBuffer, 32);
		pItemData->dwRealmData[1] = BITMANIP_Read(pBuffer, 32);

		if (dwVersion > 93)
		{
			BITMANIP_Read(pBuffer, 32);
		}
	}

	if (dwVersion < 93)
	{
		b109 = TRUE;
	}

	pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);

	if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_ANY_ARMOR))
	{
		pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_ARMORCLASS);
		nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
		STATLIST_SetUnitStat(pItem, STAT_ARMORCLASS, nValue, 0);

		STATLIST_SetUnitStat(pItem, STAT_TOBLOCK, pItemsTxtRecord->nBlock, 0);
		STATLIST_SetUnitStat(pItem, STAT_VELOCITYPERCENT, -pItemsTxtRecord->dwSpeed, 0);

		pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_MAXDURABILITY);
		nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
		STATLIST_SetUnitStat(pItem, STAT_MAXDURABILITY, nValue, 0);

		if (nValue)
		{
			pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_DURABILITY);

			nBits = (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits);

			if (dwVersion <= 95)
			{
				nBits = 8;
			}

			nValue = BITMANIP_Read(pBuffer, nBits) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
			STATLIST_SetUnitStat(pItem, STAT_DURABILITY, nValue, 0);
		}
	}
	else if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_WEAPON))
	{
		STATLIST_SetUnitStat(pItem, STAT_ATTACKRATE, -pItemsTxtRecord->dwSpeed, 0);

		if (pItemData->dwQualityNo == ITEMQUAL_INFERIOR)
		{
			if (pItemsTxtRecord->nMaxDam)
			{
				nValue = 3 * pItemsTxtRecord->nMaxDam / 4;
				if (nValue < 2)
				{
					nValue = 2;
				}
				STATLIST_SetUnitStat(pItem, STAT_MAXDAMAGE, nValue, 0);
			}

			if (pItemsTxtRecord->nMinDam)
			{
				nValue = 3 * pItemsTxtRecord->nMinDam / 4;
				if (nValue < 1)
				{
					nValue = 1;
				}
				STATLIST_SetUnitStat(pItem, STAT_MINDAMAGE, nValue, 0);
			}

			if (pItemsTxtRecord->n2HandMaxDam)
			{
				nValue = 3 * pItemsTxtRecord->n2HandMaxDam / 4;
				if (nValue < 2)
				{
					nValue = 2;
				}
				STATLIST_SetUnitStat(pItem, STAT_SECONDARY_MAXDAMAGE, nValue, 0);
			}

			if (pItemsTxtRecord->n2HandMinDam)
			{
				nValue = 3 * pItemsTxtRecord->n2HandMinDam / 4;
				if (nValue < 1)
				{
					nValue = 1;
				}
				STATLIST_SetUnitStat(pItem, STAT_SECONDARY_MINDAMAGE, nValue, 0);
			}

			if (pItemsTxtRecord->nMaxMisDam)
			{
				nValue = 3 * pItemsTxtRecord->nMinMisDam / 4;
				if (nValue < 1)
				{
					nValue = 1;
				}
				STATLIST_SetUnitStat(pItem, STAT_ITEM_THROW_MINDAMAGE, nValue, 0);

				nValue = 3 * pItemsTxtRecord->nMaxMisDam / 4;
				if (nValue < 2)
				{
					nValue = 2;
				}
				STATLIST_SetUnitStat(pItem, STAT_ITEM_THROW_MAXDAMAGE, nValue, 0);
			}
		}
		else
		{
			STATLIST_SetUnitStat(pItem, STAT_MAXDAMAGE, pItemsTxtRecord->nMaxDam, 0);
			STATLIST_SetUnitStat(pItem, STAT_MINDAMAGE, pItemsTxtRecord->nMinDam, 0);

			STATLIST_SetUnitStat(pItem, STAT_SECONDARY_MAXDAMAGE, pItemsTxtRecord->n2HandMaxDam, 0);
			STATLIST_SetUnitStat(pItem, STAT_SECONDARY_MINDAMAGE, pItemsTxtRecord->n2HandMinDam, 0);

			if (pItemsTxtRecord->nMaxMisDam)
			{
				STATLIST_SetUnitStat(pItem, STAT_ITEM_THROW_MINDAMAGE, pItemsTxtRecord->nMinMisDam, 0);
				STATLIST_SetUnitStat(pItem, STAT_ITEM_THROW_MAXDAMAGE, pItemsTxtRecord->nMaxMisDam, 0);
			}
		}

		if (pItemData->dwItemFlags & IFLAG_ETHEREAL)
		{
			STATLIST_SetUnitStat(pItem, STAT_MINDAMAGE, 3 * STATLIST_GetUnitBaseStat(pItem, STAT_MINDAMAGE, 0) / 2, 0);
			STATLIST_SetUnitStat(pItem, STAT_MAXDAMAGE, 3 * STATLIST_GetUnitBaseStat(pItem, STAT_MAXDAMAGE, 0) / 2, 0);
			STATLIST_SetUnitStat(pItem, STAT_SECONDARY_MINDAMAGE, 3 * STATLIST_GetUnitBaseStat(pItem, STAT_SECONDARY_MINDAMAGE, 0) / 2, 0);
			STATLIST_SetUnitStat(pItem, STAT_SECONDARY_MAXDAMAGE, 3 * STATLIST_GetUnitBaseStat(pItem, STAT_SECONDARY_MAXDAMAGE, 0) / 2, 0);
			STATLIST_SetUnitStat(pItem, STAT_ITEM_THROW_MINDAMAGE, 3 * STATLIST_GetUnitBaseStat(pItem, STAT_ITEM_THROW_MINDAMAGE, 0) / 2, 0);
			STATLIST_SetUnitStat(pItem, STAT_ITEM_THROW_MAXDAMAGE, 3 * STATLIST_GetUnitBaseStat(pItem, STAT_ITEM_THROW_MAXDAMAGE, 0) / 2, 0);
		}

		pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_MAXDURABILITY);
		nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
		STATLIST_SetUnitStat(pItem, STAT_MAXDURABILITY, nValue, 0);

		if (nValue)
		{
			pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_DURABILITY);

			nBits = (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits);

			if (dwVersion <= 95)
			{
				nBits = 8;
			}

			nValue = BITMANIP_Read(pBuffer, nBits) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
			STATLIST_SetUnitStat(pItem, STAT_DURABILITY, nValue, 0);
		}
	}
	else if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_GOLD))
	{
		if (BITMANIP_Read(pBuffer, 1))
		{
			nBits = 32;
		}
		else
		{
			nBits = 12;
		}

		STATLIST_SetUnitStat(pItem, STAT_GOLD, BITMANIP_Read(pBuffer, nBits), 0);
	}

	if (pItemsTxtRecord->nStackable)
	{
		if (dwVersion > 80)
		{
			nBits = 9;
		}
		else
		{
			nBits = 8;
		}

		STATLIST_SetUnitStat(pItem, STAT_QUANTITY, BITMANIP_Read(pBuffer, nBits), 0);
	}
	
	if (pItemData->dwItemFlags & IFLAG_SOCKETED)
	{
		pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_ITEM_NUMSOCKETS);

		nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits));

		nMaxSockets = pItemsTxtRecord->nInvWidth * pItemsTxtRecord->nInvHeight;

		if (nMaxSockets > 0)
		{
			if (nMaxSockets > 6)
			{
				nMaxSockets = 6;
			}

			if (nMaxSockets > ITEMS_GetMaxSockets(pItem))
			{
				nMaxSockets = ITEMS_GetMaxSockets(pItem);
			}

			if (nValue < 1)
			{
				nValue = 1;
			}

			if (nValue > nMaxSockets)
			{
				nValue = nMaxSockets;
			}

			ITEMS_SetItemFlag(pItem, IFLAG_SOCKETED, TRUE);
			STATLIST_SetUnitStat(pItem, STAT_ITEM_NUMSOCKETS, nValue, 0);
		}
	}

	if (!bCheckForHeader && !(pItemData->dwItemFlags & IFLAG_IDENTIFIED))
	{
		return 2 * (bError == 0) - 1;
	}

	if (dwVersion > 84 && pItemData->dwQualityNo == ITEMQUAL_SET)
	{
		nStatLists = 5;
		nSetItemMask = BITMANIP_Read(pBuffer, 5);
	}
	else
	{
		nStatLists = 0;
		nSetItemMask = 0;
	}

	if (bRuneword)
	{
		++nStatLists;
	}

	for (int nStatListCounter = -1; nStatListCounter < nStatLists; ++nStatListCounter)
	{
		if (nStatListCounter < 0)
		{
			nState = 0;
			nFlag = 0x40;
		}
		else if (bRuneword && nStatListCounter == nStatLists - 1)
		{
			nState = STATE_RUNEWORD;
			nFlag = 0x40;
		}
		else if (nSetItemMask & (1 << nStatListCounter))
		{
			nState = gnItemSetStates[nStatListCounter];
			nFlag = 0x2040;
		}
		else
		{
			continue;
		}
		
		pStatList = STATLIST_GetStatListFromUnitStateOrFlag(pItem, nState, nFlag);
		if (!pStatList)
		{
			pStatList = STATLIST_AllocStatList(pItem->pMemoryPool, nFlag, 0, UNIT_ITEM, pItem->dwUnitId);
			D2COMMON_10475_PostStatToStatList(pItem, pStatList, 1);
			STATLIST_SetState(pStatList, nState);
		}

		nLastStatId = -1;
		while (1)
		{
			nStatId = BITMANIP_Read(pBuffer, 9);
			if (nStatId == 511)
			{
				break;
			}

			if (!nStatId && !nLastStatId)
			{
				bError = TRUE;
				break;
			}

			nLastStatId = nStatId;

			pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nStatId);
			if (!pItemStatCostTxtRecord)
			{
				break;
			}

			switch (nStatId)
			{
			case STAT_STRENGTH:
			case STAT_DEXTERITY:
			{
				nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, nStatId, nValue, 0);
				break;
			}
			case STAT_VITALITY:
			{
				nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_VITALITY, nValue, 0);

				if (dwVersion <= 85)
				{
					pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_MAXHP);

					nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
					STATLIST_SetStatIfListIsValid(pStatList, STAT_MAXHP, nValue << 8, 0);
				}
				break;
			}
			case STAT_ENERGY:
			{
				nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_ENERGY, nValue, 0);

				if (dwVersion <= 85)
				{
					pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_MAXMANA);

					nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
					STATLIST_SetStatIfListIsValid(pStatList, STAT_MAXMANA, nValue, 0); //TODO: nValue << 8?
				}
				break;
			}
			case STAT_ITEM_MAXDAMAGE_PERCENT:
			{
				ITEMS_SetDefenseOrDamage(pItem, STAT_ITEM_MAXDAMAGE_PERCENT);

				nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_ITEM_MAXDAMAGE_PERCENT, nValue, 0);

				ITEMS_SetDefenseOrDamage(pItem, STAT_ITEM_MINDAMAGE_PERCENT);

				pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_ITEM_MINDAMAGE_PERCENT);

				nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_ITEM_MINDAMAGE_PERCENT, nValue, 0);
				break;
			}
			case STAT_FIREMINDAM:
			{
				nBits = (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits);
				if (dwVersion <= 81)
				{
					nBits = 6;
				}

				nValue = BITMANIP_Read(pBuffer, nBits) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_FIREMINDAM, nValue, 0);

				pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_FIREMAXDAM);

				nBits = (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits);
				if (dwVersion <= 81)
				{
					nBits = 7;
				}

				nValue = BITMANIP_Read(pBuffer, nBits) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_FIREMAXDAM, nValue, 0);
				break;
			}
			case STAT_LIGHTMINDAM:
			{
				nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_LIGHTMINDAM, nValue, 0);

				pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_LIGHTMAXDAM);

				nBits = (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits);
				if (dwVersion <= 81)
				{
					nBits = 7;
				}

				nValue = BITMANIP_Read(pBuffer, nBits) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_LIGHTMAXDAM, nValue, 0);
				break;
			}
			case STAT_MAGICMINDAM:
			{
				nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_MAGICMINDAM, nValue, 0);

				pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_MAGICMAXDAM);

				nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_MAGICMAXDAM, nValue, 0);
				break;
			}
			case STAT_COLDMINDAM:
			{
				nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_COLDMINDAM, nValue, 0);

				pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_COLDMAXDAM);

				nBits = (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits);
				if (dwVersion <= 81)
				{
					nBits = 7;
				}

				nValue = BITMANIP_Read(pBuffer, nBits) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_COLDMAXDAM, nValue, 0);

				pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_COLDLENGTH);

				nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_COLDLENGTH, nValue, 0);
				break;
			}
			case STAT_POISONMINDAM:
			{
				nBits = (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits);
				if (dwVersion <= 81)
				{
					nBits = 7;
				}

				nValue = BITMANIP_Read(pBuffer, nBits) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_POISONMINDAM, nValue, 0);

				pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_POISONMAXDAM);

				nBits = (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits);
				if (dwVersion <= 81)
				{
					nBits = 8;
				}

				nValue = BITMANIP_Read(pBuffer, nBits) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_POISONMAXDAM, nValue, 0);

				pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_POISONLENGTH);

				nValue = BITMANIP_Read(pBuffer, (b109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits)) - (b109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_POISONLENGTH, nValue, 0);
				STATLIST_SetStatIfListIsValid(pStatList, STAT_POISON_COUNT, 1, 0);
				break;
			}
			case STAT_ITEM_FREEZE:
			{
				if (dwVersion > 89)
				{
					ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, STAT_ITEM_FREEZE, dwVersion, b109);
				}
				else
				{
					BITMANIP_Read(pBuffer, 16);
					STATLIST_SetStatIfListIsValid(pStatList, STAT_ITEM_FREEZE, 1, 0);
				}
				break;
			}
			case STAT_ITEM_ADDCLASSSKILLS:
			{
				if (dwVersion > 92)
				{
					ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, STAT_ITEM_ADDCLASSSKILLS, dwVersion, b109);
				}
				else
				{
					STATLIST_SetStat(pStatList, STAT_ITEM_ADDCLASSSKILLS, BITMANIP_Read(pBuffer, 3), 0);
				}
				break;
			}
			case STAT_UNSENTPARAM1:
			{
				if (dwVersion > 92)
				{
					ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, STAT_UNSENTPARAM1, dwVersion, b109);
				}
				else
				{
					STATLIST_SetStat(pStatList, STAT_ITEM_ADDCLASSSKILLS, BITMANIP_Read(pBuffer, 3), 3);
				}
				break;
			}
			case STAT_ITEM_ADDEXPERIENCE:
			{
				if (dwVersion > 92)
				{
					ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, STAT_ITEM_ADDEXPERIENCE, dwVersion, b109);
				}
				else
				{
					STATLIST_SetStat(pStatList, STAT_ITEM_ADDCLASSSKILLS, BITMANIP_Read(pBuffer, 3), 2);
				}
				break;
			}
			case STAT_ITEM_HEALAFTERKILL:
			{
				if (dwVersion > 92)
				{
					ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, STAT_ITEM_HEALAFTERKILL, dwVersion, b109);
				}
				else
				{
					STATLIST_SetStat(pStatList, STAT_ITEM_ADDCLASSSKILLS, BITMANIP_Read(pBuffer, 3), 1);
				}
				break;
			}
			case STAT_ITEM_REDUCEDPRICES:
			{
				if (dwVersion > 92)
				{
					ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, STAT_ITEM_REDUCEDPRICES, dwVersion, b109);
				}
				else
				{
					STATLIST_SetStat(pStatList, STAT_ITEM_ADDCLASSSKILLS, BITMANIP_Read(pBuffer, 3), 4);
				}
				break;
			}
			case STAT_ATTACK_VS_MONTYPE:
			{
				if (dwVersion > 92)
				{
					ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, STAT_ATTACK_VS_MONTYPE, dwVersion, b109);
				}
				else
				{
					STATLIST_SetStat(pStatList, STAT_ITEM_ADDCLASSSKILLS, BITMANIP_Read(pBuffer, 3), 5);
				}
				break;
			}
			case STAT_DAMAGE_VS_MONTYPE:
			{
				if (dwVersion > 92)
				{
					ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, STAT_DAMAGE_VS_MONTYPE, dwVersion, b109);
				}
				else
				{
					STATLIST_SetStat(pStatList, STAT_ITEM_ADDCLASSSKILLS, BITMANIP_Read(pBuffer, 3), 6);
				}
				break;
			}
			case STAT_ITEM_ADDSKILL_TAB:
			case STAT_UNUSED189:
			case STAT_UNUSED190:
			case STAT_UNUSED191:
			case STAT_UNUSED192:
			case STAT_UNUSED193:
			{
				if (dwVersion > 92)
				{
					ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, nStatId, dwVersion, b109);
				}
				else
				{
					D2Common_10848(BITMANIP_Read(pBuffer, 10), &v238, &v237, &nValue);
					if (nValue > 7)
					{
						nValue = 7;
					}
					STATLIST_SetStat(pStatList, STAT_ITEM_ADDSKILL_TAB, nValue, v237 + 8 * v238);
				}
				break;
			}
			case STAT_ITEM_ELEMSKILL:
			{
				if (dwVersion > 92)
				{
					ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, STAT_ITEM_ELEMSKILL, dwVersion, b109);
				}
				else
				{
					STATLIST_SetStat(pStatList, STAT_ITEM_ELEMSKILL, BITMANIP_Read(pBuffer, 4), 1);
				}
				break;
			}
			case STAT_ITEM_SINGLESKILL:
			case STAT_ITEM_RESTINPEACE:
			case STAT_CURSE_RESISTANCE:
			case STAT_FADE:
			case STAT_ARMOR_OVERRIDE_PERCENT:
			case STAT_UNUSED183:
			case STAT_UNUSED184:
			case STAT_UNUSED185:
			case STAT_UNUSED186:
			case STAT_UNUSED187:
			{
				if (dwVersion > 92)
				{
					ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, nStatId, dwVersion, b109);
				}
				else
				{
					D2Common_10844_ITEMMODS_First(BITMANIP_Read(pBuffer, 14), &nSkillId, &nValue);

					pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_ITEM_SINGLESKILL);

					if (pItemStatCostTxtRecord)
					{
						nMax = (1 << pItemStatCostTxtRecord->nSaveBits) - 1;
					}
					else
					{
						nMax = 7;
					}

					if (nValue > 0 && nValue <= nMax)
					{
						pSkillsTxtRecord = DATATBLS_GetSkillsTxtRecord(nSkillId);
						if (pSkillsTxtRecord && DATATBLS_GetSkillDescTxtRecord(pSkillsTxtRecord->wSkillDesc))
						{
							STATLIST_SetStat(pStatList, STAT_ITEM_SINGLESKILL, nValue, nSkillId);
						}
					}
				}
				break;
			}
			case STAT_ITEM_SKILLONATTACK:
			case STAT_ITEM_SKILLONKILL:
			case STAT_ITEM_SKILLONDEATH:
			{
				if (dwVersion > 92)
				{
					ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, nStatId, dwVersion, b109);
				}
				else
				{
					D2Common_10845(BITMANIP_Read(pBuffer, 21), &v241, &v240, &v242);
					STATLIST_SetStat(pStatList, STAT_ITEM_SKILLONATTACK, v242, ((uint16_t)v241 << 6) + (v240 & 0x3F));
				}
				break;
			}
			case STAT_ITEM_SKILLONHIT:
			case STAT_ITEM_SKILLONLEVELUP:
			case STAT_UNUSED200:
			{
				if (dwVersion > 92)
				{
					ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, nStatId, dwVersion, b109);
				}
				else
				{
					D2Common_10845(BITMANIP_Read(pBuffer, 21), &v244, &v243, &v245);
					STATLIST_SetStat(pStatList, STAT_ITEM_SKILLONHIT, v245, ((uint16_t)v244 << 6) + (v243 & 0x3F));
				}
				break;
			}
			case STAT_ITEM_SKILLONGETHIT:
			case STAT_UNUSED202:
			case STAT_UNUSED203:
			{
				if (dwVersion > 92)
				{
					ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, nStatId, dwVersion, b109);
				}
				else
				{
					D2Common_10845(BITMANIP_Read(pBuffer, 21), &v247, &v246, &v248);
					STATLIST_SetStat(pStatList, STAT_ITEM_SKILLONGETHIT, v248, ((uint16_t)v247 << 6) + (v246 & 0x3F));
				}
				break;
			}
			case STAT_ITEM_CHARGED_SKILL:
			case STAT_UNUSED204:
			case STAT_UNUSED205:
			case STAT_UNUSED206:
			case STAT_UNUSED207:
			case STAT_UNUSED208:
			case STAT_UNUSED209:
			case STAT_UNUSED210:
			case STAT_UNUSED211:
			case STAT_UNUSED212:
			{
				if (dwVersion > 92)
				{
					ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, nStatId, dwVersion, b109);
				}
				else
				{
					if (dwVersion > 73)
					{
						nBits = 30;
					}
					else
					{
						nBits = 29;
					}

					D2Common_10846(BITMANIP_Read(pBuffer, nBits), &v249, &v250, &v251, &v252);

					STATLIST_SetStat(pStatList, STAT_ITEM_CHARGED_SKILL, (v252 << 8) + (uint8_t)v251, (v250 & LOWORD(sgptDataTables->nShiftedStuff)) + ((uint16_t)v249 << sgptDataTables->nStuff));
				}
				break;
			}
			default:
			{
				ITEMS_ReadStatFromItemBitstream(pBuffer, pStatList, pItemStatCostTxtRecord, nStatId, dwVersion, b109);
				break;
			}
			}
		}
			
		if (dwVersion <= 92)
		{
			nValue = STATLIST_GetStatValue(pStatList, STAT_ITEM_ADDCLASSSKILLS, 0);

			if (nValue > 0)
			{
				if (nValue == STATLIST_GetStatValue(pStatList, STAT_ITEM_ADDCLASSSKILLS, 1) && nValue == STATLIST_GetStatValue(pStatList, STAT_ITEM_ADDCLASSSKILLS, 2)
					&& nValue == STATLIST_GetStatValue(pStatList, STAT_ITEM_ADDCLASSSKILLS, 3) && nValue == STATLIST_GetStatValue(pStatList, STAT_ITEM_ADDCLASSSKILLS, 4))
				{
					for (int i = 0; i < 7; ++i)
					{
						STATLIST_SetStatIfListIsValid(pStatList, STAT_ITEM_ADDCLASSSKILLS, 0, i);
					}

					STATLIST_SetStatIfListIsValid(pStatList, STAT_ITEM_ALLSKILLS, nValue, 0);
				}
			}
		}
	}

	return 2 * (bError == FALSE) - 1;
}

// 1.13c D2Common+0x29D90 (retail: n109 in EAX, record in ESI)
//D2Common.0x6FDA29D0
void __fastcall ITEMS_ReadStatFromItemBitstream(D2BitBufferStrc* pBuffer, D2StatListStrc* pStatList, D2ItemStatCostTxt* pItemStatCostTxtRecord, int nStatId, uint32_t dwVersion, int n109)
{
	int nSaveAdd = n109 ? pItemStatCostTxtRecord->dw09SaveAdd : pItemStatCostTxtRecord->dwSaveAdd;
	int nParamBits = n109 ? pItemStatCostTxtRecord->dw09SaveParamBits : pItemStatCostTxtRecord->dwSaveParamBits;
	int nSaveBits = n109 ? pItemStatCostTxtRecord->n09SaveBits : pItemStatCostTxtRecord->nSaveBits;
	int nParam = 0;
	int nValue = 0;

	switch (nStatId)
	{
	case STAT_SECONDARY_MINDAMAGE:
		if (dwVersion <= 89)
		{
			nSaveBits = 5;
		}
		break;
	case STAT_SECONDARY_MAXDAMAGE:
		if (dwVersion <= 89)
		{
			nSaveBits = 6;
		}
		break;
	case STAT_HPREGEN:
		if (dwVersion <= 89)
		{
			nSaveBits = 5;
			nSaveAdd = 10;
		}
		break;
	case STAT_ITEM_GOLDBONUS:
		if (dwVersion <= 89)
		{
			nSaveBits = 8;
			nSaveAdd = 20;
		}
		break;
	case STAT_ITEM_MAGICBONUS:
		if (dwVersion <= 0x59)
		{
			nSaveBits = 7;
			nSaveAdd = 20;
		}
		break;
	case STAT_ITEM_FASTERATTACKRATE:
	case STAT_ITEM_FASTERMOVEVELOCITY:
	case STAT_ITEM_FASTERGETHITRATE:
	case STAT_ITEM_FASTERBLOCKRATE:
	case STAT_ITEM_FASTERCASTRATE:
		if (dwVersion <= 87)
		{
			nSaveBits = 6;
		}
		break;
	case STAT_ITEM_LEVELREQ:
	case STAT_ITEM_LEVELREQPCT:
		if (dwVersion < 93)
		{
			nStatId = 93;
		}
		break;
	case STAT_LASTBLOCKFRAME:
	case STAT_ITEM_NONCLASSSKILL:
		if (dwVersion < 93)
		{
			nStatId = 96;
		}
		break;
	case STAT_STATE:
	case STAT_MONSTER_PLAYERCOUNT:
		if (dwVersion < 93)
		{
			nStatId = 99;
		}
		break;
	case STAT_SKILL_POISON_OVERRIDE_LENGTH:
	case STAT_SKILL_BYPASS_UNDEAD:
		if (dwVersion < 93)
		{
			nStatId = 102;
		}
		break;
	case STAT_SKILL_BYPASS_DEMONS:
	case STAT_SKILL_BYPASS_BEASTS:
		if (dwVersion < 93)
		{
			nStatId = 105;
		}
		break;
	default:
		break;
	}
	if (nParamBits > 0)
	{
		nParam = BITMANIP_Read(pBuffer, nParamBits);
	}

	nValue = BITMANIP_Read(pBuffer, nSaveBits);
	if (pStatList)
	{
		STATLIST_SetStat(pStatList, nStatId, (nValue - nSaveAdd) << pItemStatCostTxtRecord->nValShift, nParam);
	}
}

//D2Common.0x6FDA2BA0 (#10881)
// 1.13c D2Common+0x29D00
size_t __stdcall ITEMS_SerializeItemToBitstream(D2UnitStrc* pItem, uint8_t* pBitstream, size_t nSize, BOOL bServer, BOOL bSaveItemInv, BOOL bGamble)
{
	D2BitBufferStrc pBuffer = {};

	BITMANIP_Initialize(&pBuffer, pBitstream, nSize);

	ITEMS_SerializeItem(pItem, &pBuffer, bServer, bSaveItemInv, bGamble);

	if (pBuffer.bFull)
	{
		return 0;
	}
	else
	{
		return BITMANIP_GetSize(&pBuffer);
	}
}

//Inlined in D2Common.0x6FDA2C00
// 1.13c D2Common+0x25C10 (retail looks the items.txt row up itself; the caller passes it here)
void __fastcall ITEMS_SerializeItemCompact(D2UnitStrc* pItem, D2BitBufferStrc* pBuffer, D2ItemsTxt* pItemsTxtRecord, BOOL bServer)
{
	D2ItemStatCostTxt* pItemStatCostTxtRecord = NULL;
	char* szName = NULL;
	int nItemFormat = 0;
	int nValue = 0;
	int nGold = 0;
	int nX = 0;
	int nY = 0;

	int v39; // [sp+24h] [bp-8h]@63
	int v40; // [sp+28h] [bp-4h]@63


	nItemFormat = ITEMS_GetItemFormat(pItem);
	if (nItemFormat > 0)
	{
		if (nItemFormat >= 1023)
		{
			nItemFormat = 1023;
		}
	}
	else
	{
		nItemFormat = 0;
	}
	BITMANIP_Write(pBuffer, nItemFormat, 10);

	ITEMS_WriteBitsToBitstream(pBuffer, pItem->dwAnimMode, 3);

	if (pItem->dwAnimMode == IMODE_ONGROUND || pItem->dwAnimMode == IMODE_DROPPING)
	{
		ITEMS_WriteBitsToBitstream(pBuffer, UNITS_GetXPosition(pItem), 16);
		ITEMS_WriteBitsToBitstream(pBuffer, UNITS_GetYPosition(pItem), 16);
	}
	else
	{
		ITEMS_WriteBitsToBitstream(pBuffer, ITEMS_GetBodyLocation(pItem), 4);

		nX = UNITS_GetXPosition(pItem);
		if (nX > 0)
		{
			if (nX >= 15)
			{
				nX = 15;
			}
		}
		else
		{
			nX = 0;
		}
		ITEMS_WriteBitsToBitstream(pBuffer, nX, 4);

		nY = UNITS_GetYPosition(pItem);
		if (nY > 0)
		{
			if (nY >= 15)
			{
				nY = 15;
			}
		}
		else
		{
			nY = 0;
		}
		ITEMS_WriteBitsToBitstream(pBuffer, nY, 4);

		ITEMS_WriteBitsToBitstream(pBuffer, (int8_t)(ITEMS_GetInvPage(pItem) + 1), 3);   // retail: signed byte arithmetic -- INVPAGE_NULL (255) serialises as 0 (s04 corpus); a page of 127 or above is negative and clamps to 0
	}

	if (ITEMS_GetItemFlags(pItem) & IFLAG_ISEAR)
	{
		ITEMS_WriteBitsToBitstream(pBuffer, ITEMS_GetFileIndex(pItem), 3);

		ITEMS_WriteBitsToBitstream(pBuffer, ITEMS_GetEarLevel(pItem), 7);

		szName = ITEMS_GetEarName(pItem);
		do
		{
			ITEMS_WriteBitsToBitstream(pBuffer, *szName, 7);
		}
		while (*szName++);
	}
	else
	{
		ITEMS_WriteBitsToBitstream(pBuffer, ITEMS_GetBaseCode(pItem), 32);

		if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_GOLD))
		{
			nGold = STATLIST_UnitGetStatValue(pItem, STAT_GOLD, 0);
			ITEMS_WriteBitsToBitstream(pBuffer, (nGold >= 4096), 1);
			ITEMS_WriteBitsToBitstream(pBuffer, nGold, (((nGold < 4096) - 1) & 20) + 12);
		}
	}

	if (pItemsTxtRecord->nQuest && pItemsTxtRecord->nQuestDiffCheck)
	{
		pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_QUESTITEMDIFFICULTY);
		D2_ASSERT(pItemStatCostTxtRecord);

		nValue = STATLIST_UnitGetStatValue(pItem, STAT_QUESTITEMDIFFICULTY, 0);
		ITEMS_WriteBitsToBitstream(pBuffer, pItemStatCostTxtRecord->dwSaveAdd + (nValue >> pItemStatCostTxtRecord->nValShift), pItemStatCostTxtRecord->nSaveBits);
	}

	if (bServer)
	{
		v40 = 0;
		v39 = 0;
		ITEMS_GetRealmData(pItem, &v40, &v39);
		if (v39)
		{
			ITEMS_WriteBitsToBitstream(pBuffer, 1, 1);
			ITEMS_WriteBitsToBitstream(pBuffer, v40, 32);
			ITEMS_WriteBitsToBitstream(pBuffer, v39, 32);
			ITEMS_WriteBitsToBitstream(pBuffer, 0, 32);
		}
		else
		{
			ITEMS_WriteBitsToBitstream(pBuffer, 0, 1);
		}
	}
}

//D2Common.0x6FDA2C00
// 1.13c D2Common+0x28D70
size_t __fastcall ITEMS_SerializeItem(D2UnitStrc* pItem, D2BitBufferStrc* pBuffer, BOOL bServer, BOOL bSaveItemInv, BOOL bGamble)
{
	// retail has no unit-type guard: the flags come from the item data (0 when absent) and the
	// items.txt row (0x222B0) is fatal when the class id is out of range
	D2ItemDataStrc* pItemData = ITEMS_GetItemData(pItem);
	uint32_t nItemFlags = pItemData ? pItemData->dwItemFlags : 0;

	nItemFlags = nItemFlags & ~IFLAG_INIT | IFLAG_JUSTSAVED;

	D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);

	if (pItemsTxtRecord->nCompactSave)
	{
		nItemFlags |= IFLAG_COMPACTSAVE;
	}

	if (bGamble)
	{
		nItemFlags = nItemFlags & ~IFLAG_ETHEREAL | IFLAG_LOWQUALITY;
	}

	if (bServer)
	{
		BITMANIP_Write(pBuffer, 'MJ', 16);
	}
	else
	{
		if (!(nItemFlags & IFLAG_IDENTIFIED))
		{
			nItemFlags &= ~IFLAG_SOCKETED;
		}
	}
	BITMANIP_Write(pBuffer, nItemFlags, 32);

	if (nItemFlags & IFLAG_COMPACTSAVE)
	{
		ITEMS_SerializeItemCompact(pItem, pBuffer, pItemsTxtRecord, bServer);
	}
	else
	{
		ITEMS_SerializeItemComplete(pItem, pBuffer, bServer, bGamble);
	}

	if (!bGamble && bSaveItemInv && pItem->pInventory)
	{
		for (D2UnitStrc* i = INVENTORY_GetFirstItem(pItem->pInventory); i; i = INVENTORY_GetNextItem(i))
		{
			BITMANIP_GoToNextByte(pBuffer);
			ITEMS_SerializeItem(INVENTORY_UnitIsItem(i), pBuffer, bServer, bSaveItemInv, 0);
		}
	}

	return BITMANIP_GetSize(pBuffer);
}

// 1.13c D2Common+0x23DD0 (retail: nBits in ESI)
//D2Common.0x6FDA2FD0
void __fastcall ITEMS_WriteBitsToBitstream(D2BitBufferStrc* pBuffer, int nData, int nBits)
{
	if (nBits != 32)
	{
		nData &= (nData <= 0) - 1;
		if (nData >= (1 << nBits) - 1)
		{
			nData = (1 << nBits) - 1;
		}
	}

	return BITMANIP_Write(pBuffer, nData, nBits);
}

// 1.13c D2Common+0x27180
//D2Common.0x6FDA3010
void __fastcall ITEMS_SerializeItemComplete(D2UnitStrc* pItem, D2BitBufferStrc* pBuffer, BOOL bServer, BOOL bGamble)
{
	static const int gnItemSetStates_6FDD15A8[] =
	{
		STATE_ITEMSET1, STATE_ITEMSET2, STATE_ITEMSET3, STATE_ITEMSET4, STATE_ITEMSET5
	};

	D2ItemStatCostTxt* pItemStatCostTxtRecord = NULL;
	D2ItemStatCostTxt* pLocalRecord = NULL;
	D2MagicAffixDataTbl* pMagicAffixInfo = NULL;
	D2ItemTypesTxt* pItemTypesTxtRecord = NULL;
	D2ItemsTxt* pItemsTxtRecord = NULL;
	D2StatListStrc* pStatListEx = NULL;
	D2UnitStrc* pInventoryItem = NULL;
	char* szPlayerName = NULL;
	int nMagicPrefixOffset = 0;
	int nAutoMagicOffset = 0;
	int nStatListCounter = 0;
	int nSocketedItems = 0;
	int nMaxDurability = 0;
	int nItemFormat = 0;
	int nItemLevel = 0;
	int nItemType = 0;
	int nQuality = 0;
	int nInvGfx = 0;
	int nVarInvGfx = 0;
	int nAnimMode = 0;
	int nBodyLoc = 0;
	int nStatCounter = 0;
	int nSetItemMask = 0;
	int nMagicPrefix = 0;
	int nMagicSuffix = 0;
	int nRarePrefix = 0;
	int nRareSuffix = 0;
	int nRunewordId = 0;
	int nScrollType = 0;
	int nAutoAffix = 0;
	int nStatLists = 0;
	int nFileIndex = 0;
	int nEarLevel = 0;
	int nCounter = 0;
	int nStatId = 0;
	int nStats = 0;
	int nValue = 0;
	int nGold = 0;
	int nStorePage = 0;
	BOOL bIsRuneword = FALSE;
	BOOL bInvalid = FALSE;
	D2CoordStrc pCoords = {};
	int nStatValues[511] = {};
	D2StatStrc pStat[511] = {};

	D2ItemDataStrc* pItemData = ITEMS_GetItemData(pItem);
	D2_ASSERT(pItemData);

	nItemFormat = pItemData->wItemFormat;
	if (nItemFormat > 0)
	{
		if (nItemFormat >= 1023)
		{
			nItemFormat = 1023;
		}
	}
	else
	{
		nItemFormat = 0;
	}
	BITMANIP_Write(pBuffer, nItemFormat, 10);

	nAnimMode = pItem->dwAnimMode;
	if (nAnimMode > 0)
	{
		if (nAnimMode >= 7)
		{
			nAnimMode = 7;
		}
	}
	else
	{
		nAnimMode = 0;
	}
	BITMANIP_Write(pBuffer, nAnimMode, 3);

	if (nAnimMode == IMODE_ONGROUND || nAnimMode == IMODE_DROPPING)
	{
		UNITS_GetCoords(pItem, &pCoords);

		if (pCoords.nX > 0)
		{
			if (pCoords.nX >= 65535)
			{
				pCoords.nX = 65535;
			}
		}
		else
		{
			pCoords.nX = 0;
		}
		BITMANIP_Write(pBuffer, pCoords.nX, 16);

		if (pCoords.nY > 0)
		{
			if (pCoords.nY >= 65535)
			{
				pCoords.nY = 65535;
			}
		}
		else
		{
			pCoords.nY = 0;
		}
		BITMANIP_Write(pBuffer, pCoords.nY, 16);
	}
	else
	{
		nBodyLoc = pItemData->nBodyLoc;
		if (nBodyLoc > 0)
		{
			if (nBodyLoc >= 15)
			{
				nBodyLoc = 15;
			}
		}
		else
		{
			nBodyLoc = 0;
		}
		BITMANIP_Write(pBuffer, nBodyLoc, 4);

		UNITS_GetCoords(pItem, &pCoords);

		if (pCoords.nX > 0)
		{
			if (pCoords.nX >= 15)
			{
				pCoords.nX = 15;
			}
		}
		else
		{
			pCoords.nX = 0;
		}
		BITMANIP_Write(pBuffer, pCoords.nX, 4);

		if (pCoords.nY > 0)
		{
			if (pCoords.nY >= 15)
			{
				pCoords.nY = 15;
			}
		}
		else
		{
			pCoords.nY = 0;
		}
		BITMANIP_Write(pBuffer, pCoords.nY, 4);

		nStorePage = int8_t(pItemData->nInvPage + 1); // d2-web 1.13c: signed-char arithmetic
		if (nStorePage > 0)
		{
			if (nStorePage >= 7)
			{
				nStorePage = 7;
			}
		}
		else
		{
			nStorePage = 0;
		}
		BITMANIP_Write(pBuffer, nStorePage, 3);
	}

	pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
	D2_ASSERT(pItemsTxtRecord);

	if (bGamble)
	{
		if (pItemsTxtRecord->dwNormCode)
		{
			BITMANIP_Write(pBuffer, pItemsTxtRecord->dwNormCode, 32);
		}
		else
		{
			BITMANIP_Write(pBuffer, pItemsTxtRecord->dwCode, 32);
		}
		return;
	}

	BITMANIP_Write(pBuffer, pItemsTxtRecord->dwCode, 32);

	nSocketedItems = 0;
	if (ITEMS_CheckIfSocketable(pItem) && pItem->pInventory)
	{
		pInventoryItem = INVENTORY_GetFirstItem(pItem->pInventory);
		if (pInventoryItem)
		{
			do
			{
				++nSocketedItems;
				pInventoryItem = INVENTORY_GetNextItem(pInventoryItem);
			}
			while (pInventoryItem);

			if (nSocketedItems > 0)
			{
				if (nSocketedItems >= 7)
				{
					nSocketedItems = 7;
				}
			}
			else
			{
				nSocketedItems = 0;
			}
		}
	}
	BITMANIP_Write(pBuffer, nSocketedItems, 3);

	if (bServer)
	{
		BITMANIP_Write(pBuffer, pItem->dwInitSeed, 32);
	}

	if ((int)pItemData->dwItemLevel < 1)
	{
		pItemData->dwItemLevel = 1;
	}
		
	nItemLevel = pItemData->dwItemLevel;

	if (nItemLevel > 0)
	{
		if (nItemLevel >= 99)
		{
			nItemLevel = 99; // d2-web 1.13c: item level saturates at 99 (1.10f: 127)
		}
	}
	else
	{
		nItemLevel = 0;
	}
	BITMANIP_Write(pBuffer, nItemLevel, 7);

	nQuality = pItemData->dwQualityNo;
	if (nQuality > 0)
	{
		if (nQuality >= 15)
		{
			nQuality = 15;
		}
	}
	else
	{
		nQuality = 0;
	}
	BITMANIP_Write(pBuffer, nQuality, 4);
	
	nItemType = ITEMS_GetItemType(pItem);
	pItemTypesTxtRecord = DATATBLS_GetItemTypesTxtRecord(nItemType);
	if (pItemTypesTxtRecord)
	{
		nVarInvGfx = pItemTypesTxtRecord->nVarInvGfx;
	}
	else
	{
		nVarInvGfx = 0;
	}
	BITMANIP_Write(pBuffer, nVarInvGfx ? 1 : 0, 1);

	if (pItemTypesTxtRecord && pItemTypesTxtRecord->nVarInvGfx)
	{
		nInvGfx = pItemData->nInvGfxIdx;
		if (nInvGfx > 0)
		{
			if (nInvGfx >= 7)
			{
				nInvGfx = 7;
			}
		}
		else
		{
			nInvGfx = 0;
		}
		BITMANIP_Write(pBuffer, nInvGfx, 3);
	}

	pMagicAffixInfo = DATATBLS_GetMagicAffixDataTables();
	nMagicPrefixOffset = static_cast<int>(pMagicAffixInfo->pMagicPrefix - pMagicAffixInfo->pMagicAffixTxt);
	nAutoMagicOffset = static_cast<int>(pMagicAffixInfo->pAutoMagic - pMagicAffixInfo->pMagicAffixTxt);

	nAutoAffix = pItemData->wAutoAffix;
	if (nAutoAffix > nAutoMagicOffset)
	{
		nAutoAffix -= nAutoMagicOffset;
	}
	BITMANIP_Write(pBuffer, nAutoAffix ? 1 : 0, 1);

	if (nAutoAffix)
	{
		if (nAutoAffix > 0)
		{
			if (nAutoAffix >= 2047)
			{
				nAutoAffix = 2047;
			}
		}
		else
		{
			nAutoAffix = 0;
		}
		BITMANIP_Write(pBuffer, nAutoAffix, 11);
	}

	switch (pItemData->dwQualityNo)
	{
	case ITEMQUAL_INFERIOR:
	case ITEMQUAL_SUPERIOR:
	{
		nFileIndex = pItemData->dwFileIndex;
		if (nFileIndex > 0)
		{
			if (nFileIndex >= 7)
			{
				nFileIndex = 7;
			}
		}
		else
		{
			nFileIndex = 0;
		}
		BITMANIP_Write(pBuffer, nFileIndex, 3);
		break;
	}
	case ITEMQUAL_RARE:
	case ITEMQUAL_CRAFT:
	{
		if (bServer || pItemData->dwItemFlags & IFLAG_IDENTIFIED)
		{
			nRarePrefix = pItemData->wRarePrefix;
			if (nRarePrefix > 0)
			{
				if (nRarePrefix >= 255)
				{
					nRarePrefix = 255;
				}
			}
			else
			{
				nRarePrefix = 0;
			}
			BITMANIP_Write(pBuffer, nRarePrefix, 8);

			nRareSuffix = pItemData->wRareSuffix;
			if (nRareSuffix > 0)
			{
				if (nRareSuffix >= 255)
				{
					nRareSuffix = 255;
				}
			}
			else
			{
				nRareSuffix = 0;
			}
			BITMANIP_Write(pBuffer, nRareSuffix, 8);
		}

		nCounter = 0;
		do
		{
			nMagicPrefix = pItemData->wMagicPrefix[nCounter];
			if (nMagicPrefix > nMagicPrefixOffset)
			{
				nMagicPrefix -= nMagicPrefixOffset;
			}

			if (nMagicPrefix)
			{
				BITMANIP_Write(pBuffer, 1, 1);

				if (nMagicPrefix > 0)
				{
					if (nMagicPrefix >= 2047)
					{
						nMagicPrefix = 2047;
					}
				}
				else
				{
					nMagicPrefix = 0;
				}
				BITMANIP_Write(pBuffer, nMagicPrefix, 11);
			}
			else
			{
				BITMANIP_Write(pBuffer, 0, 1);
			}

			if (pItemData->wMagicSuffix[nCounter] != 0)
			{
				BITMANIP_Write(pBuffer, 1, 1);

				nMagicSuffix = pItemData->wMagicSuffix[nCounter];
				if (nMagicSuffix > 0)
				{
					if (nMagicSuffix >= 2047)
					{
						nMagicSuffix = 2047;
					}
				}
				else
				{
					nMagicSuffix = 0;
				}
				BITMANIP_Write(pBuffer, nMagicSuffix, 11);
			}
			else
			{
				BITMANIP_Write(pBuffer, 0, 1);
			}

			++nCounter;
		}
		while (nCounter < ITEMS_MAX_MODS);

		break;
	}
	case ITEMQUAL_TEMPERED:
	{
		if (bServer || pItemData->dwItemFlags & IFLAG_IDENTIFIED)
		{
			nRarePrefix = pItemData->wRarePrefix;
			if (nRarePrefix > 0)
			{
				if (nRarePrefix >= 255)
				{
					nRarePrefix = 255;
				}
			}
			else
			{
				nRarePrefix = 0;
			}
			BITMANIP_Write(pBuffer, nRarePrefix, 8);

			nRareSuffix = pItemData->wRareSuffix;
			if (nRareSuffix > 0)
			{
				if (nRareSuffix >= 255)
				{
					nRareSuffix = 255;
				}
			}
			else
			{
				nRareSuffix = 0;
			}
			BITMANIP_Write(pBuffer, nRareSuffix, 8);
		}
		break;
	}
	case ITEMQUAL_MAGIC:
	{
		if (bServer || pItemData->dwItemFlags & IFLAG_IDENTIFIED)
		{
			nMagicPrefix = pItemData->wMagicPrefix[0];
			if (nMagicPrefix > nMagicPrefixOffset)
			{
				nMagicPrefix -= nMagicPrefixOffset;
			}

			if (nMagicPrefix > 0)
			{
				if (nMagicPrefix >= 2047)
				{
					nMagicPrefix = 2047;
				}
			}
			else
			{
				nMagicPrefix = 0;
			}
			BITMANIP_Write(pBuffer, nMagicPrefix, 11);

			nMagicSuffix = pItemData->wMagicSuffix[0];
			if (nMagicSuffix > 0)
			{
				if (nMagicSuffix >= 2047)
				{
					nMagicSuffix = 2047;
				}

			}
			else
			{
				nMagicSuffix = 0;
			}
			BITMANIP_Write(pBuffer, nMagicSuffix, 11);
		}
		break;
	}
	case ITEMQUAL_SET:
	case ITEMQUAL_UNIQUE:
	{
		if (bServer || pItemData->dwItemFlags & IFLAG_IDENTIFIED)
		{
			nFileIndex = pItemData->dwFileIndex;
			if (nFileIndex < 0)
			{
				nFileIndex = 4095;
			}
			if (nFileIndex >= 4095)
			{
				nFileIndex = 4095;
			}
			BITMANIP_Write(pBuffer, nFileIndex, 12);
		}
		break;
	}
	default:
	{
		if (pItemData->dwQualityNo != ITEMQUAL_NORMAL)
		{
			pItemData->dwQualityNo = ITEMQUAL_NORMAL;
			bInvalid = TRUE;
		}

		if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_CHARM) && (bServer || pItemData->dwItemFlags & IFLAG_IDENTIFIED))
		{
			nMagicPrefix = pItemData->wMagicPrefix[0];
			if (nMagicPrefix > nMagicPrefixOffset)
			{
				nMagicPrefix -= nMagicPrefixOffset;
			}
			BITMANIP_Write(pBuffer, nMagicPrefix ? 1 : 0, 1);

			if (nMagicPrefix)
			{
				if (nMagicPrefix > 0)
				{
					if (nMagicPrefix >= 2047)
					{
						nMagicPrefix = 2047;
					}
					BITMANIP_Write(pBuffer, nMagicPrefix, 11);
				}
				else
				{
					BITMANIP_Write(pBuffer, 0, 11);
				}
			}
			else
			{
				nMagicSuffix = pItemData->wMagicSuffix[0];
				if (nMagicSuffix > 0)
				{
					if (nMagicSuffix >= 2047)
					{
						nMagicSuffix = 2047;
					}
					BITMANIP_Write(pBuffer, nMagicSuffix, 11);
				}
				else
				{
					BITMANIP_Write(pBuffer, 0, 11);
				}
			}
		}

		if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_BODY_PART) && !ITEMS_CheckItemTypeId(pItem, ITEMTYPE_PLAYER_BODY_PART))
		{
			nFileIndex = pItemData->dwFileIndex;
			if (nFileIndex > 0)
			{
				if (nFileIndex >= 1023)
				{
					nFileIndex = 1023;
				}
			}
			else
			{
				nFileIndex = 0;
			}
			BITMANIP_Write(pBuffer, nFileIndex, 10);
		}

		if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_SCROLL) || ITEMS_CheckItemTypeId(pItem, ITEMTYPE_BOOK))
		{
			nScrollType = pItemData->wMagicSuffix[0];
			if (nScrollType > 0)
			{
				if (nScrollType >= 31)
				{
					nScrollType = 31;
				}
			}
			else
			{
				nScrollType = 0;
			}
			BITMANIP_Write(pBuffer, nScrollType, 5);
		}
		break;
	}
	}

	if (pItemData->dwItemFlags & IFLAG_RUNEWORD)
	{
		if (const D2RunesTxt* pRunesTxtRecord = ITEMS_GetRunesTxtRecordFromItem(pItem))
		{
			nRunewordId = pRunesTxtRecord->wStringId;
		}
		else
		{
			nRunewordId = 65535; // d2-web 1.13c: missing runes row writes 0xFFFF
		}

		if (nRunewordId > 0)
		{
			if (nRunewordId >= 65535)
			{
				nRunewordId = 65535;
			}
		}
		else
		{
			nRunewordId = 0;
		}
		BITMANIP_Write(pBuffer, nRunewordId, 16);
	}

	if (pItemData->dwItemFlags & IFLAG_ISEAR)
	{
		nFileIndex = pItemData->dwFileIndex;
		if (nFileIndex > 0)
		{
			if (nFileIndex >= 7)
			{
				nFileIndex = 7;
			}
		}
		else
		{
			nFileIndex = 0;
		}
		BITMANIP_Write(pBuffer, nFileIndex, 3);

		nEarLevel = pItemData->nEarLvl;
		if (nEarLevel > 0)
		{
			if (nEarLevel >= 127)
			{
				nEarLevel = 127;
			}
		}
		else
		{
			nEarLevel = 0;
		}
		BITMANIP_Write(pBuffer, nEarLevel, 7);

		szPlayerName = pItemData->szPlayerName;
		do
		{
			int nCurrentCharacter = D2Clamp<int>(int8_t(*szPlayerName), 0, 127);
			BITMANIP_Write(pBuffer, nCurrentCharacter, 7);
		}
		while (*szPlayerName++);
	}
	else if (pItemData->dwItemFlags & IFLAG_PERSONALIZED)
	{
		szPlayerName = pItemData->szPlayerName;
		do
		{
			int nCurrentCharacter = D2Clamp<int>(int8_t(*szPlayerName), 0, 127);
			BITMANIP_Write(pBuffer, nCurrentCharacter, 7);
		}
		while (*szPlayerName++);
	}

	if (bServer)
	{
		if (pItemData->dwRealmData[1])
		{
			BITMANIP_Write(pBuffer, 1, 1);
			BITMANIP_Write(pBuffer, pItemData->dwRealmData[0], 32);
			ITEMS_WriteBitsToBitstream(pBuffer, pItemData->dwRealmData[1], 32);
			ITEMS_WriteBitsToBitstream(pBuffer, 0, 32);
		}
		else
		{
			ITEMS_WriteBitsToBitstream(pBuffer, 0, 1);
		}
	}

	if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_ANY_ARMOR))
	{
		pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_ARMORCLASS);
		ITEMS_WriteBitsToBitstream(pBuffer, STATLIST_GetUnitBaseStat(pItem, STAT_ARMORCLASS, 0) + pItemStatCostTxtRecord->dwSaveAdd, pItemStatCostTxtRecord->nSaveBits);

		pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_MAXDURABILITY);
		nMaxDurability = STATLIST_GetUnitBaseStat(pItem, STAT_MAXDURABILITY, 0);
		ITEMS_WriteBitsToBitstream(pBuffer, nMaxDurability + pItemStatCostTxtRecord->dwSaveAdd, pItemStatCostTxtRecord->nSaveBits);
		if (nMaxDurability)
		{
			pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_DURABILITY);
			ITEMS_WriteBitsToBitstream(pBuffer, STATLIST_UnitGetStatValue(pItem, STAT_DURABILITY, 0) + pItemStatCostTxtRecord->dwSaveAdd, pItemStatCostTxtRecord->nSaveBits);
		}
	}
	else if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_WEAPON))
	{
		pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_MAXDURABILITY);
		nMaxDurability = STATLIST_GetUnitBaseStat(pItem, STAT_MAXDURABILITY, 0);
		ITEMS_WriteBitsToBitstream(pBuffer, nMaxDurability + pItemStatCostTxtRecord->dwSaveAdd, pItemStatCostTxtRecord->nSaveBits);

		if (nMaxDurability)
		{
			pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(STAT_DURABILITY);
			ITEMS_WriteBitsToBitstream(pBuffer, STATLIST_UnitGetStatValue(pItem, STAT_DURABILITY, 0) + pItemStatCostTxtRecord->dwSaveAdd, pItemStatCostTxtRecord->nSaveBits);
		}
	}
	else if (ITEMS_CheckItemTypeId(pItem, ITEMTYPE_GOLD))
	{
		nGold = STATLIST_UnitGetStatValue(pItem, STAT_GOLD, 0);
		ITEMS_WriteBitsToBitstream(pBuffer, nGold >= 4096, 1);
		ITEMS_WriteBitsToBitstream(pBuffer, nGold, nGold >= 4096 ? 32 : 12);
	}

	if (ITEMS_CheckIfStackable(pItem))
	{
		ITEMS_WriteBitsToBitstream(pBuffer, STATLIST_UnitGetStatValue(pItem, STAT_QUANTITY, 0), 9);
	}

	if (pItemData->dwItemFlags & IFLAG_SOCKETED)
	{
		ITEMS_WriteBitsToBitstream(pBuffer, STATLIST_GetUnitBaseStat(pItem, STAT_ITEM_NUMSOCKETS, 0), ITEMS_GetItemStatCostTxtRecord(STAT_ITEM_NUMSOCKETS)->nSaveBits);
	}

	if (bServer || pItemData->dwItemFlags & IFLAG_IDENTIFIED)
	{
		nStatLists = 0;
		if (ITEMS_GetItemQuality(pItem) == ITEMQUAL_SET)
		{
			nSetItemMask = 0;
			nCounter = 0;
			do
			{
				if (STATLIST_GetStatListFromUnitStateAndFlag(pItem, gnItemSetStates_6FDD15A8[nCounter], 0x2040) || STATLIST_GetStatListFromUnitStateAndFlag(pItem, gnItemSetStates_6FDD15A8[nCounter], 0x40))
				{
					nStatLists = nCounter + 1;
					nSetItemMask += 1 << nCounter;
				}

				++nCounter;
			}
			while (nCounter < 5);

			ITEMS_WriteBitsToBitstream(pBuffer, nSetItemMask, 5);
		}

		bIsRuneword = 0;
		if (ITEMS_CheckItemFlag(pItem, IFLAG_RUNEWORD, __LINE__, __FILE__))
		{
			++nStatLists;
			bIsRuneword = TRUE;
		}

		nStatListCounter = -1;

		pStatListEx = STATLIST_GetStatListFromUnitStateAndFlag(pItem, 0, 0x40);

		while (1)
		{
			if (pStatListEx && !bInvalid)
			{
				memset(nStatValues, 0, sizeof(nStatValues));
				nStats = STATLIST_GetBaseStatsData((D2StatListExStrc*)pStatListEx, pStat, ARRAY_SIZE(pStat));

				nStatCounter = 0;
				while (nStatCounter < nStats)
				{
					nStatId = pStat[nStatCounter].nStat;

					pItemStatCostTxtRecord = ITEMS_GetItemStatCostTxtRecord(nStatId);
					nValue = pStat[nStatCounter].nValue >> pItemStatCostTxtRecord->nValShift;

					if (pItemStatCostTxtRecord->nSaveBits && nValue && nValue != nStatValues[nStatId])
					{
						ITEMS_WriteBitsToBitstream(pBuffer, nStatId, 9);

						if (nStatId <= STAT_MAGICMINDAM)
						{
							if (nStatId == STAT_MAGICMINDAM)
							{
								ITEMS_WriteBitsToBitstream(pBuffer, nValue + pItemStatCostTxtRecord->dwSaveAdd, pItemStatCostTxtRecord->nSaveBits);

								pLocalRecord = ITEMS_GetItemStatCostTxtRecord(STAT_MAGICMAXDAM);
								nStatValues[STAT_MAGICMAXDAM] = STATLIST_GetStatValue(pStatListEx, STAT_MAGICMAXDAM, 0);
								ITEMS_WriteBitsToBitstream(pBuffer, nStatValues[STAT_MAGICMAXDAM] + pLocalRecord->dwSaveAdd, pLocalRecord->nSaveBits);
							}
							else if (nStatId == STAT_ITEM_MAXDAMAGE_PERCENT)
							{
								ITEMS_WriteBitsToBitstream(pBuffer, nValue + pItemStatCostTxtRecord->dwSaveAdd, pItemStatCostTxtRecord->nSaveBits);

								pLocalRecord = ITEMS_GetItemStatCostTxtRecord(STAT_ITEM_MINDAMAGE_PERCENT);
								nStatValues[STAT_ITEM_MINDAMAGE_PERCENT] = STATLIST_GetStatValue(pStatListEx, STAT_ITEM_MINDAMAGE_PERCENT, 0);
								ITEMS_WriteBitsToBitstream(pBuffer, nStatValues[STAT_ITEM_MINDAMAGE_PERCENT] + pLocalRecord->dwSaveAdd, pLocalRecord->nSaveBits);
							}
							else if (nStatId == STAT_FIREMINDAM)
							{
								ITEMS_WriteBitsToBitstream(pBuffer, nValue + pItemStatCostTxtRecord->dwSaveAdd, pItemStatCostTxtRecord->nSaveBits);

								pLocalRecord = ITEMS_GetItemStatCostTxtRecord(STAT_FIREMAXDAM);
								nStatValues[STAT_FIREMAXDAM] = STATLIST_GetStatValue(pStatListEx, STAT_FIREMAXDAM, 0);
								ITEMS_WriteBitsToBitstream(pBuffer, nStatValues[STAT_FIREMAXDAM] + pLocalRecord->dwSaveAdd, pLocalRecord->nSaveBits);
							}
							else if (nStatId == STAT_LIGHTMINDAM)
							{
								ITEMS_WriteBitsToBitstream(pBuffer, nValue + pItemStatCostTxtRecord->dwSaveAdd, pItemStatCostTxtRecord->nSaveBits);

								pLocalRecord = ITEMS_GetItemStatCostTxtRecord(STAT_LIGHTMAXDAM);
								nStatValues[STAT_LIGHTMAXDAM] = STATLIST_GetStatValue(pStatListEx, STAT_LIGHTMAXDAM, 0);
								ITEMS_WriteBitsToBitstream(pBuffer, nStatValues[STAT_LIGHTMAXDAM] + pLocalRecord->dwSaveAdd, pLocalRecord->nSaveBits);
							}
							else
							{
								if ((int)pItemStatCostTxtRecord->dwSaveParamBits > 0)
								{
									ITEMS_WriteBitsToBitstream(pBuffer, pStat[nStatCounter].nLayer, pItemStatCostTxtRecord->dwSaveParamBits);
								}

								if (pItemStatCostTxtRecord->nSaveBits)
								{
									ITEMS_WriteBitsToBitstream(pBuffer, nValue + pItemStatCostTxtRecord->dwSaveAdd, pItemStatCostTxtRecord->nSaveBits);
								}
							}
						}
						else if (nStatId == STAT_COLDMINDAM)
						{
							ITEMS_WriteBitsToBitstream(pBuffer, nValue + pItemStatCostTxtRecord->dwSaveAdd, pItemStatCostTxtRecord->nSaveBits);

							pLocalRecord = ITEMS_GetItemStatCostTxtRecord(STAT_COLDMAXDAM);
							nStatValues[STAT_COLDMAXDAM] = STATLIST_GetStatValue(pStatListEx, STAT_COLDMAXDAM, 0);
							ITEMS_WriteBitsToBitstream(pBuffer, nStatValues[STAT_COLDMAXDAM] + pLocalRecord->dwSaveAdd, pLocalRecord->nSaveBits);

							pLocalRecord = ITEMS_GetItemStatCostTxtRecord(STAT_COLDLENGTH);
							nStatValues[STAT_COLDLENGTH] = STATLIST_GetStatValue(pStatListEx, STAT_COLDLENGTH, 0);
							ITEMS_WriteBitsToBitstream(pBuffer, nStatValues[STAT_COLDLENGTH] + pLocalRecord->dwSaveAdd, pLocalRecord->nSaveBits);
						}
						else if (nStatId == STAT_POISONMINDAM)
						{
							ITEMS_WriteBitsToBitstream(pBuffer, nValue + pItemStatCostTxtRecord->dwSaveAdd, pItemStatCostTxtRecord->nSaveBits);

							pLocalRecord = ITEMS_GetItemStatCostTxtRecord(STAT_POISONMAXDAM);
							nStatValues[STAT_POISONMAXDAM] = STATLIST_GetStatValue(pStatListEx, STAT_POISONMAXDAM, 0);
							ITEMS_WriteBitsToBitstream(pBuffer, nStatValues[STAT_POISONMAXDAM] + pLocalRecord->dwSaveAdd, pLocalRecord->nSaveBits);

							pLocalRecord = ITEMS_GetItemStatCostTxtRecord(STAT_POISONLENGTH);
							nStatValues[STAT_POISONLENGTH] = STATLIST_GetStatValue(pStatListEx, STAT_POISONLENGTH, 0);
							ITEMS_WriteBitsToBitstream(pBuffer, nStatValues[STAT_POISONLENGTH] + pLocalRecord->dwSaveAdd, pLocalRecord->nSaveBits);
						}
						else if (nStatId != STAT_POISON_COUNT)
						{
							if ((int)pItemStatCostTxtRecord->dwSaveParamBits > 0)
							{
								ITEMS_WriteBitsToBitstream(pBuffer, pStat[nStatCounter].nLayer, pItemStatCostTxtRecord->dwSaveParamBits);
							}

							if (pItemStatCostTxtRecord->nSaveBits)
							{
								ITEMS_WriteBitsToBitstream(pBuffer, nValue + pItemStatCostTxtRecord->dwSaveAdd, pItemStatCostTxtRecord->nSaveBits);
							}
						}
					}

					++nStatCounter;
				}
			}

			if (nStatListCounter < 0 || pStatListEx || bIsRuneword)
			{
				ITEMS_WriteBitsToBitstream(pBuffer, 511, 9);
			}

			++nStatListCounter;

			if (nStatListCounter >= nStatLists)
			{
				return;
			}

			if (bIsRuneword && nStatListCounter == nStatLists - 1)
			{
				pStatListEx = STATLIST_GetStatListFromUnitStateOrFlag(pItem, STATE_RUNEWORD, 0x40);
			}
			else
			{
				pStatListEx = STATLIST_GetStatListFromUnitStateAndFlag(pItem, gnItemSetStates_6FDD15A8[nStatListCounter], 0x2040);
				if (!pStatListEx)
				{
					pStatListEx = STATLIST_GetStatListFromUnitStateAndFlag(pItem, gnItemSetStates_6FDD15A8[nStatListCounter], 0x40);
				}
			}
		}
	}
}

//D2Common.0x6FDA42B0
// 1.13c D2Common+0x17A0 (row index in EAX; no null test of the table pointer, as in retail)
D2ItemStatCostTxt* __fastcall ITEMS_GetItemStatCostTxtRecord(int nStatId)
{
	if (nStatId >= 0 && nStatId < sgptDataTables->nItemStatCostTxtRecordCount)
	{
		return &sgptDataTables->pItemStatCostTxt[nStatId];
	}

	return NULL;
}

// 1.13c D2Common+0x23D70
//D2Common.0x6FDA42E0 (#10837)
int __stdcall ITEMS_GetNoOfSetItemsFromItem(D2UnitStrc* pItem)
{
	if (D2SetItemsTxt* pSetItemsTxtRecord = ITEMS_GetSetItemsTxtRecordFromItem(pItem))
	{
		return pSetItemsTxtRecord->nSetItems;
	}
	return -1;
}


//D2Common.0x6FDD15C8
int gnItemSetStates[] =
{
	STATE_ITEMSET1, STATE_ITEMSET2, STATE_ITEMSET3, STATE_ITEMSET4, STATE_ITEMSET5, STATE_ITEMSET6
};

//D2Common.0x6FDD15E0
int gnSetMaskToBonusMappingTable[] =
{
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6
};

D2SetItemsTxt* D2Common_1800_GetSetItemsTxtRecord(int32_t nSetItemId);
void D2Common_3A330_AddStatLayer(D2StatListStrc* pStatList, int32_t nStatId, int32_t nDelta, uint16_t nLayer);
static_assert(offsetof(D2SetItemsTxt, nSetId) == 0x2C, "setitems.txt nSetId");

// 1.13c D2Common+0x2C7C0 (SetItemToggleStates; retail: pItem in EBX, nSetItemMask on the stack)
// Only a set-quality item with a set record and a nonzero nAddFunc does anything. nAddFunc 1 sets or clears each
// state except the item's own set rank; nAddFunc 2 clears the states below the bonus count, then re-posts the rest
// of the owner's state lists (SET 0x2000) unless that flag is already present. Returns 1 once it ran, else 0.
BOOL __fastcall sub_6FDA4380(D2UnitStrc* pItemS, unsigned int nSetItemMask)
{
	if (pItemS == nullptr)
		return 0;

	if (pItemS->dwUnitType != 4 || ITEMS_GetItemQuality(pItemS) != 5)
		return 0;

	D2SetItemsTxt* pSetItemsTxt = ITEMS_GetSetItemsTxtRecordFromItem(pItemS);
	if (pSetItemsTxt == nullptr || pSetItemsTxt->nAddFunc == 0)
		return 0;

	if (pSetItemsTxt->nAddFunc == 1)
	{
		const int32_t nSetItems = ITEMS_GetNoOfSetItemsFromItem(pItemS);
		for (int32_t i = 0; i < 6; ++i)
		{
			if (i == nSetItems)
				continue;

			int32_t nIndex = i;
			if (nSetItems < i)
				--nIndex;

			const bool bUnset = (nSetItemMask & (1u << i)) == 0;
			D2Common_10574(pItemS, gnItemSetStates[nIndex], bUnset ? 1 : 0);
		}
	}
	else if (pSetItemsTxt->nAddFunc == 2)
	{
		const int32_t nBonuses = nSetItemMask < 64
			? gnSetMaskToBonusMappingTable[nSetItemMask] : 0;
		const int32_t nFirst = nBonuses - 1;

		for (int32_t i = 0; i < nFirst; ++i)
			D2Common_10574(pItemS, gnItemSetStates[i], 0);

		if (nFirst >= 5)
			return 1;

		for (int32_t i = nFirst; i < 5; ++i)
		{
			// With zero bonuses retail starts one word before the table; that slot reads as zero.
			const int32_t nState = i < 0 ? 0 : gnItemSetStates[i];
			D2StatListExStrc* pStatListEx = pItemS->pStatListEx;
			if (nState == 0 || pStatListEx == nullptr || static_cast<int32_t>(pStatListEx->dwFlags) >= 0)
				continue;

			D2StatListStrc* pStateList = D2Common_GetStateFromStatListEx_6FDB8190(pStatListEx, nState);
			if (pStateList == nullptr || (pStateList->dwFlags & 0x2000u) != 0)
				continue;

			D2Common_ExpireStatList_6FDB6E30(pStateList);
			pStateList->dwFlags |= 0x2000u;
			D2COMMON_10475_PostStatToStatList(pItemS, pStateList, 1);
		}
	}

	return 1;
}

// 1.13c D2Common+0x2C630 (retail: pUnit in EAX): apply (nMode 0) or remove (1) one set item's bonus on its owner.
// The item must be a set item; an owner that is itself an item must not be magic/set/rare/unique/crafted/tempered.
// The six set-state lists are searched for the one whose STAT_VALUE holds the set id, remembering the first empty
// state; add into an empty state allocates a list, posts it and stamps the state; remove expires and frees the list.
BOOL __fastcall sub_6FDA4490(D2UnitStrc* pUnit, D2UnitStrc* pItem, int nMode)
{
	if (nMode >= 2 || pUnit == nullptr || pItem == nullptr)
		return 0;
	if (pItem->dwUnitType != UNIT_ITEM || ITEMS_GetItemQuality(pItem) != ITEMQUAL_SET)
		return 0;
	if (pUnit->dwUnitType == UNIT_ITEM && ITEMS_IsMagSetRarUniCrfOrTmp(pUnit) != 0)
		return 0;

	D2SetItemsTxt* pSetItemsTxt = D2Common_1800_GetSetItemsTxtRecord(ITEMS_GetFileIndex(pItem));
	if (pSetItemsTxt == nullptr)
		return 0;
	const int32_t nSetId = pSetItemsTxt->nSetId;
	if (nSetId < 0 || nSetId >= sgptDataTables->nSetsTxtRecordCount)
		return 0;

	int32_t nEmptyState = -1;
	int32_t nState = 0;
	D2StatListStrc* pStatList = nullptr;
	bool bFound = false;
	for (int32_t i = 0; i < 6; ++i)
	{
		nState = gnItemSetStates[i];
		pStatList = STATLIST_GetStatListFromUnitAndState(pUnit, nState);
		if (pStatList == nullptr)
		{
			if (nEmptyState < 0)
				nEmptyState = i;
			continue;
		}
		if (nSetId == STATLIST_GetStatValue(pStatList, STAT_VALUE, 0))
		{
			bFound = true;
			if (nMode == 0)
				STATLIST_RemoveAllStats(pStatList);
			break;
		}
	}

	if (!bFound)
	{
		if (nMode == 1 || nEmptyState < 0)
			return 0;
		nState = gnItemSetStates[nEmptyState];
		pStatList = STATLIST_AllocStatList(pUnit->pMemoryPool, 0, 0, static_cast<int>(pItem->dwUnitType), pItem->dwUnitId);
		D2COMMON_10475_PostStatToStatList(pUnit, pStatList, 1);
		if (pStatList != nullptr)
			STATLIST_SetState(pStatList, nState);
	}

	if (nMode == 0)
	{
		D2Common_3A330_AddStatLayer(pStatList, STAT_VALUE, nSetId, 0);
		ITEMMODS_UpdateFullSetBoni(pUnit, pItem, nState);
	}
	else
	{
		D2Common_ExpireStatList_6FDB6E30(pStatList);
		STATLIST_FreeStatList(pStatList);
	}
	return 1;
}

// 1.13c D2Common+0x2C910
//D2Common.0x6FDA4640 (#10866)
BOOL __stdcall ITEMS_UpdateSets(D2UnitStrc* pUnit, D2UnitStrc* pItem, int nMode, int bForceStates)
{
	if (pUnit == nullptr)
		return 0;
	if (pItem == nullptr || pItem->dwUnitType != UNIT_ITEM || pUnit->pInventory == nullptr)
		return 0;

	unsigned int nSetItemMask = 0;
	// 0x1E880 hands back the item when the inventory owns it; 0x1E8C0 reads its node page.
	if (INVENTORY_IsItemInInventory(pUnit->pInventory, pItem) != nullptr
		&& INVENTORY_GetItemNodePage(pItem) == 3)
		nSetItemMask = ITEMS_GetSetItemsMask(pUnit, pItem, TRUE); // 0x23F90

	if (bForceStates != 0 || nMode == 0)
		sub_6FDA4380(pItem, nSetItemMask);

	sub_6FDA4490(pUnit, pItem, nMode);
	return 1;
}

// 1.13c D2Common+0x222B0: items.txt nCompactSave byte of an item's class. Retail's out-of-range and no-table
// paths call _exit(-1); this returns 0 for them and the capture never reaches them.
uint8_t D2Common_222B0(const D2UnitStrc* pItem)
{
	const uint32_t nClass = pItem ? pItem->dwClassId : 0xFFFFFFFFu;
	const D2ItemDataTbl& t = sgptDataTables->pItemDataTables;
	if (nClass < (uint32_t)t.nItemsTxtRecordCount && t.pItemsTxt)
	{
		return t.pItemsTxt[nClass].nCompactSave;
	}
	return 0;
}

// 1.13c D2Common+0x22FC0: running maximum with one row's level byte (eax = running value, ecx = row, edx = key).
// The row's slot byte (+0x67) picks the alternate byte (+0x68) when it is not 0xFF and equals the key's dword at +4;
// otherwise the base byte (+0x65) is used. A null row returns the running value.
extern "C" int32_t D2Common_22FC0_RunningMax(int32_t running, const uint8_t* pRow, const uint8_t* pKey)
{
	if (!pRow)
	{
		return running;
	}

	const uint8_t slot = pRow[0x67];
	bool useAlternate = false;
	if (slot != 0xFF && pKey)
	{
		uint32_t keySlot = 0;
		memcpy(&keySlot, pKey + 4, sizeof(keySlot));
		useAlternate = (keySlot == slot);
	}

	const int32_t value = useAlternate ? pRow[0x68] : pRow[0x65];
	return (running > value) ? running : value;
}
