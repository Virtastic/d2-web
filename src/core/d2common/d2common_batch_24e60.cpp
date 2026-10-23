// 1.13c D2Common implementation batch: +0x24E60, +0x24610, +0x532C0, +0x4FFD0,
// +0x37E70, +0x23F90, +0x302E0, +0x2E2C0, +0x38B00, +0x2DF80, +0x120F0, +0x121D0.
// Translated from the cached 1.13c decompiles. Where a body depends on a separate UNMAPPED row,
// keep its gate and return shape here and name that dependency instead of inventing its behavior.
#include <cstdint>
#include <emscripten.h>
#include "D2DataTbls.h"
#include "D2Items.h"
#include "Units/UnitFinds.h"
#include "Fog.h"
#include "DataTbls/ItemsTbls.h"
#include "Drlg/D2DrlgDrlg.h"

extern "C" int32_t d2_fn_d2common_2ddb0(uint32_t pLevel);

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t W(const uint8_t* b, int o) { return *reinterpret_cast<const uint32_t*>(b + o); }
	inline void W(uint8_t* b, int o, uint32_t v) { *reinterpret_cast<uint32_t*>(b + o) = v; }
}

// Item unit -> ItemTypes.txt row (via 0x230F0) field +0x0E (wQuiver), sign-extended; 0 otherwise.
// 1.13c D2Common+0x24E60
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_24e60(uint32_t pUnit) {
	if (!pUnit || W(P(pUnit), 0) != 4) return 0;
	const int32_t t = static_cast<int32_t>(ITEMS_GetItemType(reinterpret_cast<D2UnitStrc*>(P(pUnit))));
	if (t < 0 || t >= sgptDataTables->nItemTypesTxtRecordCount || !sgptDataTables->pItemTypesTxt) return 0;
	return static_cast<int16_t>(sgptDataTables->pItemTypesTxt[t].wQuiver);
}

// Maximum sockets for an item unit (ITEMS_GetMaxSockets): ItemTypes row of the item's type, cap by
// item level (<=25 nMaxSock1, <=40 nMaxSock25, else nMaxSock40), min with items.txt nGemSockets.
// 1.13c D2Common+0x24610
extern "C" uint32_t d2_ours_d2common_24610(D2UnitStrc* pItem) {
	if (!pItem || pItem->dwUnitType != 4) return 0;
	const int32_t t = static_cast<int32_t>(ITEMS_GetItemType(pItem));
	if (t < 0 || t >= sgptDataTables->nItemTypesTxtRecordCount || !sgptDataTables->pItemTypesTxt) return 0;
	const D2ItemTypesTxt* r = &sgptDataTables->pItemTypesTxt[t];
	const uint32_t gem = ITEMS_GetAllowedGemSocketsFromItemId(pItem->dwClassId);
	const int lvl = ITEMS_GetItemLevel(pItem);
	const uint32_t cap = lvl < 26 ? r->nMaxSock1 : lvl < 41 ? r->nMaxSock25 : r->nMaxSock40;
	return gem < cap ? gem : cap;
}
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_24610(uint32_t pUnit) {
	return d2_ours_d2common_24610(reinterpret_cast<D2UnitStrc*>(P(pUnit)));
}

// Find a matching saved-item row in the sorted item table. Table globals are not yet modeled.
// 1.13c D2Common+0x532C0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_532c0(uint32_t pItems, uint32_t nCount, uint32_t nKind, uint32_t nMinLevel, uint32_t nExpansion) {
	(void)pItems; (void)nCount; (void)nKind; (void)nMinLevel; (void)nExpansion;
	return 0; // Retail's indexed table base/count globals are still outside the engine model.
}

// Body is byte-identical to 0x4FF80 (monster in anim mode 12, no state-mask-33 hit, MonStats2 bit 7).
// 1.13c D2Common+0x4FFD0
extern "C" uint32_t D2Common_4FF80_MonsterAnim12NoStateFlag7(D2UnitStrc* pUnit);
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_4ffd0(uint32_t pUnit) {
	return D2Common_4FF80_MonsterAnim12NoStateFlag7(reinterpret_cast<D2UnitStrc*>(P(pUnit)));
}

// Read the object's flags result and its one-bit auxiliary marker.
// 1.13c D2Common+0x37E70
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_37e70(uint32_t pUnit, uint32_t pAux) {
	if (pAux) W(P(pAux), 0, 0);
	if (!pUnit) return 0;
	const uint32_t pUnitData = W(P(pUnit), 0x5C);
	if (!pUnitData) return 0;
	const uint32_t pObjectData = W(P(pUnitData), 0x34);
	if (!pObjectData || static_cast<int32_t>(W(P(pObjectData), 0x10)) >= 0) return 0;
	if (pAux) W(P(pAux), 0, (~(W(P(pUnitData), 0x10) >> 30)) & 1);
	return W(P(pObjectData), 0x44);
}

// Build the set-item quality mask by walking inventory items and resolving their set records.
// 1.13c D2Common+0x23F90
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_23f90(uint32_t pUnit, uint32_t pItem, uint32_t nIncludeCurrent) {
	return ITEMS_GetSetItemsMask(reinterpret_cast<D2UnitStrc*>(P(pUnit)), reinterpret_cast<D2UnitStrc*>(P(pItem)), nIncludeCurrent); // vendored D2MOO body, validated in the spec
}

// Unit stat accessor for the unit's data block. These unit types store the primary value at +0x10;
// other types store the value at +0x04. Retail terminates on a null block.
// 1.13c D2Common+0x302E0
extern "C" int32_t D2Common_302E0_UnitDataValue(uint32_t pUnit) {
	if (!pUnit) return 0;
	const uint32_t nType = W(P(pUnit), 0);
	const uint32_t pData = W(P(pUnit), 0x2C);
	if (!pData) return 0;
	if (nType == 2 || (nType > 3 && nType < 6))
		return static_cast<int32_t>(W(P(pData), 0x10) << 16);
	return static_cast<int32_t>(W(P(pData), 0x04));
}

// Tick the DRLG's level list (pDrlg+0x47C, next +0x1AC; pool at pDrlg+0x478). For a level with
// +0x0C == 0 and +0x10 != 0: delay (+0x1D4) != 0 -> decrement; else if 0x2DDB0 (can-discard)
// is 0 reset the delay to 10, otherwise free the level via 0x2DAF0 (DRLG_FreeLevel, bAlloc=1).
// 1.13c D2Common+0x2E2C0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_2e2c0(uint32_t pDrlg) {
	if (!pDrlg) return;
	const uint32_t pool = W(P(pDrlg), 0x478);
	for (uint32_t p = W(P(pDrlg), 0x47C); p; p = W(P(p), 0x1AC)) {
		if (W(P(p), 0x0C) != 0 || W(P(p), 0x10) == 0) continue;
		const uint32_t nDelay = W(P(p), 0x1D4);
		if (nDelay) { W(P(p), 0x1D4, nDelay - 1); continue; }
		if (d2_fn_d2common_2ddb0(p) == 0) W(P(p), 0x1D4, 10);
		else DRLG_FreeLevel(reinterpret_cast<void*>(static_cast<uintptr_t>(pool)), reinterpret_cast<D2DrlgLevelStrc*>(static_cast<uintptr_t>(p)), 1);
	}
}

// Value of stat nStatId (EDI) in stat list pList (EBX), binary-searched in the array at +0x24
// (0x382B0); with the ItemStatCost record pRec (stack): flag bit 8 + extended list (flags<0) + owner
// type 0/1 + value < pRec->+0x2C => value = +0x2C << (+0x18 & 31). 0 if the stat is absent.
// 1.13c D2Common+0x38B00
extern "C" EMSCRIPTEN_KEEPALIVE int32_t D2Common_38B00_StatValue(uint32_t pList, int32_t nStatId, uint32_t pRec) {
	const uint8_t* l = P(pList);
	const uint8_t* arr = P(W(l, 0x24));
	int32_t hi = static_cast<int16_t>(*reinterpret_cast<const uint16_t*>(l + 0x28)), lo = 0, idx = -1;
	while (lo < hi) {
		const int32_t mid = (hi - lo) / 2 + lo;
		const int32_t v = static_cast<int32_t>(W(arr, mid * 8));
		if (v < nStatId) lo = mid + 1;
		else if (v == nStatId) { idx = mid; break; }
		else hi = mid;
	}
	if (idx < 0) return 0;
	int32_t val = static_cast<int32_t>(W(arr, idx * 8 + 4));
	if (pRec && (P(pRec)[5] & 1) && static_cast<int32_t>(W(l, 0x10)) < 0) {
		const uint32_t o = W(l, 0x44);
		if (o && (W(P(o), 0) == 0 || W(P(o), 0) == 1) && val < static_cast<int32_t>(W(P(pRec), 0x2C)))
			val = static_cast<int32_t>(W(P(pRec), 0x2C)) << (P(pRec)[0x18] & 31);
	}
	return val;
}

// Add nDelta to a level's reference count (+0x0C, clamped at 0, delay +0x1D4 = 10), then do the same
// to the level record of each nonzero id in its eight-entry link list (0x44CE0 on the DRLG at +0x1B4
// and the level id at +0x1D0), finding it in the DRLG level list (+0x47C, next +0x1AC, id +0x1D0) or
// allocating it with 0x2D9B0. pContext is retail's ESI. The 0x44CE0 seam is null until installed.
// 1.13c D2Common+0x2DF80
extern "C" int32_t* (*g_d2common_fn_44CE0)(uint32_t pDrlg, int32_t nLevelId);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_2df80(uint32_t pContext, int32_t nDelta) {
	auto bump = [nDelta](uint32_t p) {
		const int32_t v = static_cast<int32_t>(W(P(p), 0x0C)) + nDelta;
		W(P(p), 0x1D4, 10);
		W(P(p), 0x0C, v < 1 ? 0 : static_cast<uint32_t>(v));
	};
	bump(pContext);
	const uint32_t pDrlg = W(P(pContext), 0x1B4);
	if (!g_d2common_fn_44CE0) return;
	const int32_t* ids = g_d2common_fn_44CE0(pDrlg, static_cast<int32_t>(W(P(pContext), 0x1D0)));
	for (int i = 0; i < 8; ++i) {
		if (!ids[i]) continue;
		uint32_t p = W(P(pDrlg), 0x47C);
		while (p && static_cast<int32_t>(W(P(p), 0x1D0)) != ids[i]) p = W(P(p), 0x1AC);
		if (!p) p = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(
			DRLG_AllocLevel(reinterpret_cast<D2DrlgStrc*>(P(pDrlg)), ids[i])));
		bump(p);
	}
}

// Grow a UnitFinds array by fifteen entries via the pool realloc (retail takes the record in ESI;
// a null record logs the Fog error string and exits, which is not modelled).
// 1.13c D2Common+0x120F0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_120f0(D2UnitFindDataStrc* pFinds) {
	pFinds->nMaxArrayEntries += 15;
	pFinds->pUnitsArray = (D2UnitStrc**)D2_REALLOC_POOL(pFinds->pMemPool, pFinds->pUnitsArray, sizeof(D2UnitStrc*) * pFinds->nMaxArrayEntries);
}
