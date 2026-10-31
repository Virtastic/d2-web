// 1.13c D2Common+0x2C910 (ordinal 10600, 1.10f ITEMS_UpdateSets): refresh a set item's set states
// and the owner's set bonus, routed through the 1.13c helpers 0x2C7C0 and 0x2C630 rather than the
// 1.10f sub_6FDA4380 / sub_6FDA4490 the vendored ITEMS_UpdateSets still calls.
#include <cstdint>
#include <emscripten.h>

#include "D2Inventory.h"
#include "D2ItemMods.h"
#include "D2Items.h"

// Same layout as the definition in d2common_batch_87080.cpp (only sub45F90 is used here).
struct D2CommonBatch87080Env {
	void (*sub45F90)(uint32_t pUnit, uint32_t pItem, int32_t nState);
	void (*sub7BB70)(uint32_t pRoom, uint8_t nStatus);
	void (*setStatus[4])(uint32_t pRoom);
};

extern "C" int32_t d2_fn_d2common_2c630(uint32_t pUnit, uint32_t pItem, int32_t nMode,
                                        const D2CommonBatch87080Env* env);
extern "C" int32_t d2_fn_d2common_2c7c0(uint32_t pItem, uint32_t nSetItemMask);

namespace
{
	inline uint32_t A(const D2UnitStrc* p) { return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p)); }
	inline D2UnitStrc* U(uint32_t a) { return reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(a)); }

	// 0x45F90(unit, state) with the item in EBX: 1.10f ITEMMODS_UpdateFullSetBoni.
	void UpdateFullSetBoni(uint32_t pUnit, uint32_t pItem, int32_t nState)
	{
		ITEMMODS_UpdateFullSetBoni(U(pUnit), U(pItem), nState);
	}

	const D2CommonBatch87080Env kEnv{ UpdateFullSetBoni, nullptr, { nullptr, nullptr, nullptr, nullptr } };
}

// 1.13c D2Common+0x2C910
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_2c910(D2UnitStrc* pUnit, D2UnitStrc* pItem,
                                                              int32_t nMode, int32_t bForceStates)
{
	if (pUnit == nullptr)
		return 0;
	if (pItem == nullptr || pItem->dwUnitType != UNIT_ITEM || pUnit->pInventory == nullptr)
		return 0;

	uint32_t nSetItemMask = 0;
	// 0x1E880 hands back the item when the inventory owns it; 0x1E8C0 reads its node page.
	if (INVENTORY_IsItemInInventory(pUnit->pInventory, pItem) != nullptr
		&& INVENTORY_GetItemNodePage(pItem) == 3)
		nSetItemMask = ITEMS_GetSetItemsMask(pUnit, pItem, TRUE); // 0x23F90

	if (bForceStates != 0 || nMode == 0)
		d2_fn_d2common_2c7c0(A(pItem), nSetItemMask);

	d2_fn_d2common_2c630(A(pUnit), A(pItem), nMode, &kEnv);
	return 1;
}
