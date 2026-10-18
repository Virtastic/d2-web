// 1.13c D2Common implementation batch (rows 0x44350, 0x46A40, 0x82F00, 0x251C0).
// Three of the four bodies are the vendored D2MOO functions and are marked where they live
// (D2Composit.cpp, DataTbls/OverlayTbls.cpp, Items/Items.cpp); their 1.13c deltas are fixed there.
// The remaining row, 0x46A40, is written here from the cached 1.13c decompile
// (build-wasm/ghidra-d2common/decomp/0x46A40.c).
//
// 0x46A40's callees are all vendored and implemented: 0x24430 ITEMS_CheckItemTypeId,
// 0x38B00 STATLIST_GetBaseStat_6FDB6340, 0x3A280 STATLIST_SetStat and
// 0x38550 STATLIST_InsertStatModOrFail_6FDB7690. The retail call site passes the stat list and the
// packed stat id to 0x38B00 in registers, so they are supplied here (stat list = unit+0x5C, packed id
// = layer 0 of the stat being written), which is what 0x38B00's own body consumes.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "DataTbls/ItemsTbls.h"
#include "Units/Item.h"
#include "Units/Units.h"

// The helper below is shared by the branches of the 0x46A40 body.
namespace
{
	// The engine's unit and item structures are pointer-widened relative to retail's 32-bit layout,
	// so the 1.13c offsets are not asserted here; the fields are addressed by name.
	static_assert(STAT_MINDAMAGE == 0x15, "STAT_MINDAMAGE");
	static_assert(STAT_MAXDAMAGE == 0x16, "STAT_MAXDAMAGE");
	static_assert(STAT_SECONDARY_MINDAMAGE == 0x17, "STAT_SECONDARY_MINDAMAGE");
	static_assert(STAT_SECONDARY_MAXDAMAGE == 0x18, "STAT_SECONDARY_MAXDAMAGE");
	static_assert(STAT_ARMORCLASS == 0x1F, "STAT_ARMORCLASS");
	static_assert(STAT_ITEM_THROW_MINDAMAGE == 0x9F, "STAT_ITEM_THROW_MINDAMAGE");
	static_assert(STAT_ITEM_THROW_MAXDAMAGE == 0xA0, "STAT_ITEM_THROW_MAXDAMAGE");
	static_assert(ITEMTYPE_WEAPON == 0x2D, "ITEMTYPE_WEAPON");

	void SetItemBaseStat(D2UnitStrc* pUnit, int32_t nStatId)
	{
		D2StatListStrc* pStatList = pUnit->pStatListEx;

		int32_t nValue = 0;
		if (nStatId < sgptDataTables->nItemStatCostTxtRecordCount &&
		    sgptDataTables->pItemStatCostTxt != nullptr && pStatList != nullptr)
		{
			nValue = STATLIST_GetBaseStat_6FDB6340(
				pStatList,
				D2SLayerStatIdStrc::MakeFromStatId(static_cast<uint16_t>(nStatId)).nPackedValue,
				&sgptDataTables->pItemStatCostTxt[nStatId]);
		}
		nValue = (nValue * 3) / 2;

		if (STATLIST_SetStat(pStatList, nStatId, nValue, 0) && pUnit->dwUnitType == UNIT_PLAYER)
		{
			STATLIST_InsertStatModOrFail_6FDB7690(
				pStatList,
				D2SLayerStatIdStrc::Make(0, static_cast<uint16_t>(nStatId)).nPackedValue);
		}
	}
}

// 1.13c D2Common+0x46A40 (#10890)
// Seeds an item unit's base stats: an item of item type 0x2D (ITEMTYPE_WEAPON) gets the six damage
// stats below, any other item gets armour class. Each stat is written as 3/2 of the stat list's
// current base value (0 when the ItemStatCost row, the table or the list is missing); a successful
// write on a player unit also inserts the stat mod. An item unit with item data first gets
// 0x00400000 in its item flags. The retail fatal path for a null unit is modelled as a return.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_46a40(uint32_t pUnitAddress)
{
	D2UnitStrc* pUnit = reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnitAddress));
	if (pUnit == nullptr)
	{
		return;
	}

	if (pUnit->dwUnitType == UNIT_ITEM && pUnit->pItemData != nullptr)
	{
		pUnit->pItemData->dwItemFlags |= 0x00400000;
	}

	if (!ITEMS_CheckItemTypeId(pUnit, ITEMTYPE_WEAPON))
	{
		SetItemBaseStat(pUnit, STAT_ARMORCLASS);
	}
	else
	{
		SetItemBaseStat(pUnit, STAT_MINDAMAGE);
		SetItemBaseStat(pUnit, STAT_MAXDAMAGE);
		SetItemBaseStat(pUnit, STAT_SECONDARY_MINDAMAGE);
		SetItemBaseStat(pUnit, STAT_SECONDARY_MAXDAMAGE);
		SetItemBaseStat(pUnit, STAT_ITEM_THROW_MINDAMAGE);
		SetItemBaseStat(pUnit, STAT_ITEM_THROW_MAXDAMAGE);
	}
}
