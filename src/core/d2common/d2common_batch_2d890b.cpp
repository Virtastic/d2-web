// 1.13c D2Common implementation batch (rows 0x26190, 0x266E0, 0x5D800, 0x5E410, 0x5E480, 0x5E4D0).
// 0x26190 is the same code as the vendored 1.10f ITEMS_GetRequiredLevel, so it delegates. The other
// five bodies are written from the cached 1.13c decompiles. Fatal paths ending in _exit(-1) are
// modelled as returns, as in the rest of src/core/d2common.
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "D2DataTbls.h"
#include "D2Inventory.h"
#include "D2Items.h"
#include "D2Lang.h"
#include "D2StatList.h"
#include "D2Unicode.h"
#include "Units/Units.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/MonsterTbls.h"
#include "DataTbls/ObjectsTbls.h"

// Only pointer-free offsets are pinned: the engine is wasm64, so its unit and inventory layouts are
// wider than retail's 32-bit ones and the members are reached by name.
static_assert(offsetof(D2UnitStrc, dwUnitType) == 0x00, "unit type");
static_assert(offsetof(D2UnitStrc, dwClassId) == 0x04, "unit class id");
static_assert(offsetof(D2InventoryStrc, dwSignature) == 0x00, "inventory signature");
static_assert(sizeof(D2ObjectsTxt) == 0x1C0, "objects.txt stride");
static_assert(offsetof(D2ObjectsTxt, szName) == 0x00, "objects.txt name");
static_assert(sizeof(D2CharStatsTxt) == 0xC4, "charstats.txt stride");
static_assert(offsetof(D2CharStatsTxt, szClassName) == 0x20, "charstats.txt class name");
static_assert(sizeof(D2MonStatsTxt) == 0x1A8, "monstats.txt stride");
static_assert(offsetof(D2MonStatsTxt, wNameStr) == 0x06, "monstats.txt name string");
static_assert(sizeof(D2ItemsTxt) == 0x1A8, "items.txt stride");
static_assert(offsetof(D2ItemsTxt, wNameStr) == 0xF4, "items.txt name string");

int D2Common_10211_ITEMS_CheckIfSocketableByItemId(int nItemId);
int32_t STATLIST_GetTotalStatValue(D2StatListStrc* pStatList, int nStatId, uint16_t nLayer);

// 1.13c D2Common+0x26190 (#?): required level of an item unit for a player unit.
// Prior art: D2MOO 1.10f ITEMS_GetRequiredLevel (ordinal 10920), which the vendored engine already
// carries and which 0x26DA0 (ITEMS_GetLevelRequirement) forwards to; that forwarder is verified by
// s1003. This row is that body: quality switch over magic (three affixes), rare and craft (six
// prefix/suffix slots, plus the auto affix), set (setitems.txt wLvlReq) and unique (uniqueitems.txt
// wLvlReq, gated on the expansion flag or the item format); then items.txt nLevelReq (+0x13F), then
// the recursion over socketed items, then the single-skill and non-class-skill stat lists (+6 for a
// skill of another class), and finally the item-level-requirement stat cost (record 0x5C) added and
// clamped at zero. 1.13c reads the auto affix at +0x36, prefixes at +0x38..+0x42 and suffixes at
// +0x3E..+0x48, matching the 1.10f layout the vendored struct pins.
// Delta: retail only requires a non-null unit of type UNIT_ITEM to enter the body (a null item data
// pointer falls back to quality 2 and still reads items.txt); the vendored body returns 0 when the
// item data is null. An item unit without item data is not producible by the engine.
int D2Common_26190_GetItemRequiredLevel(D2UnitStrc* pItem, D2UnitStrc* pPlayer)
{
	return ITEMS_GetRequiredLevel(pItem, pPlayer);
}

// 1.13c D2Common+0x266E0 (#?): sum a stat over an item's contained (socketed) units.
// The item must be UNIT_ITEM, socketable by class id (0x22920) and carry a valid inventory
// (signature 0x1020304). With the item in mode 1, each contained item unit with a stat list adds
// STATLIST_GetTotalStatValue(stat list, nStatId, layer 0); the walk follows the item data extra-data
// pNextItem chain (item data +0x64, i.e. embedded extra data +0x08) and stops at the first non-item
// unit. A null stat argument and an out-of-range class id both yield 0. The output is zeroed first.
void D2Common_266E0_GetContainedItemsStatTotal(D2UnitStrc* pItem, int32_t nStatId, int32_t* pTotal)
{
	*pTotal = 0;

	if (!pItem || pItem->dwUnitType != UNIT_ITEM)
		return;
	if (!D2Common_10211_ITEMS_CheckIfSocketableByItemId(pItem->dwClassId))
		return;

	D2InventoryStrc* pInventory = pItem->pInventory;
	if (!pInventory || pInventory->dwSignature != 0x1020304)
		return;

	for (D2UnitStrc* pContained = INVENTORY_GetFirstItem(pInventory); pContained;
	     pContained = INVENTORY_GetNextItem(pContained))
	{
		if (pContained->dwUnitType != UNIT_ITEM)
			return;
		if (pItem->dwItemMode == 1 && pContained->pStatListEx)
			*pTotal += STATLIST_GetTotalStatValue(pContained->pStatListEx, nStatId, 0);
	}
}

// 1.13c D2Common+0x5D800 (#?): object display name.
// Object id in ECX, output buffer in EDX. An id outside objects.txt (signed compare) is fatal (0xDD
// above, 0xDE below), modelled as a return. In range, a row with a non-empty szName is copied; an
// empty name becomes "object %d".
void D2Common_5D800_GetObjectName(int32_t nObjectId, char* szOut)
{
	if (nObjectId < sgptDataTables->nObjectsTxtRecordCount)
	{
		if (nObjectId >= 0)
		{
			const D2ObjectsTxt* pRecord = sgptDataTables->pObjectsTxt + nObjectId;
			if (pRecord && pRecord->szName[0])
			{
				std::strcpy(szOut, pRecord->szName);
				return;
			}
			std::sprintf(szOut, "object %d", static_cast<int>(nObjectId));
			return;
		}
	}
	// retail fatal 0xDD / 0xDE, then _exit(-1)
}

// 1.13c D2Common+0x5E410 (#?): item display name.
// Item id in ECX, output buffer in ESI. The bound is unsigned: an id at or above the items.txt count
// (including a negative id) becomes "item %d". In range, a null items.txt table is fatal (0x1EF),
// modelled as a return; a row whose wNameStr is not the 0x1506 sentinel is narrowed from the string
// table, otherwise the id is formatted.
void D2Common_5E410_GetItemName(int32_t nItemId, char* szOut)
{
	const D2ItemDataTbl& itemData = sgptDataTables->pItemDataTables;
	if (static_cast<uint32_t>(nItemId) < static_cast<uint32_t>(itemData.nItemsTxtRecordCount))
	{
		if (!itemData.pItemsTxt)
			return; // retail fatal 0x1EF, then _exit(-1)

		const D2ItemsTxt* pRecord = itemData.pItemsTxt + nItemId;
		if (pRecord && pRecord->wNameStr != 5382)
		{
			Unicode::unicode2Win(szOut,
			                     D2LANG_GetStringFromTblIndex(pRecord->wNameStr), 0x40);
			return;
		}
	}

	std::sprintf(szOut, "item %d", static_cast<int>(nItemId));
}

// 1.13c D2Common+0x5E480 (#?): monster display name.
// The unit type in ECX is unused; the class id is in EDX, the output buffer in ESI. An id outside
// monsters.txt (signed compare) becomes "monster %d"; so does a row whose wNameStr is the 0x1506
// sentinel. Otherwise the name string is narrowed.
void D2Common_5E480_GetMonsterName(int32_t nClassId, char* szOut)
{
	if (nClassId >= 0 && nClassId < sgptDataTables->nMonStatsTxtRecordCount)
	{
		const D2MonStatsTxt* pRecord = sgptDataTables->pMonStatsTxt + nClassId;
		if (pRecord && pRecord->wNameStr != 5382)
		{
			Unicode::unicode2Win(szOut,
			                     D2LANG_GetStringFromTblIndex(pRecord->wNameStr), 0x40);
			return;
		}
	}

	std::sprintf(szOut, "monster %d", static_cast<int>(nClassId));
}

// 1.13c D2Common+0x5E4D0 (#?): player class display name.
// Class id in ECX, output buffer in EDX. An id outside charstats.txt (signed compare) or a row with
// an empty class name becomes "player %d"; otherwise the fixed class name is copied.
void D2Common_5E4D0_GetPlayerName(int32_t nClassId, char* szOut)
{
	if (nClassId >= 0 && nClassId < sgptDataTables->nCharStatsTxtRecordCount)
	{
		const D2CharStatsTxt* pRecord = sgptDataTables->pCharStatsTxt + nClassId;
		if (pRecord && pRecord->szClassName[0])
		{
			std::strcpy(szOut, pRecord->szClassName);
			return;
		}
	}

	std::sprintf(szOut, "player %d", static_cast<int>(nClassId));
}
