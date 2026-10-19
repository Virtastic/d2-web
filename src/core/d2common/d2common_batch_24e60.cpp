// 1.13c D2Common implementation batch: +0x24E60, +0x24610, +0x532C0, +0x4FFD0,
// +0x37E70, +0x23F90, +0x302E0, +0x2E2C0, +0x38B00, +0x2DF80, +0x120F0, +0x121D0.
// Translated from the cached 1.13c decompiles. Where a body depends on a separate UNMAPPED row,
// keep its gate and return shape here and name that dependency instead of inventing its behavior.
#include <cstdint>
#include <emscripten.h>

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t W(const uint8_t* b, int o) { return *reinterpret_cast<const uint32_t*>(b + o); }
	inline void W(uint8_t* b, int o, uint32_t v) { *reinterpret_cast<uint32_t*>(b + o) = v; }
}

// Item unit -> the selected SetItemsTxt record's signed value at +0x0E. The item-file-index
// helper (0x230F0) and table globals are separate rows, so this batch entry preserves the type gate.
// 1.13c D2Common+0x24E60
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_24e60(uint32_t pUnit) {
	if (!pUnit || W(P(pUnit), 0) != 4) return 0;
	return 0; // 0x230F0 and the set-item table lookup remain unimplemented dependencies.
}

// Item-quality cap lookup. Retail requires an item unit, resolves its table row (0x230F0),
// obtains its item level (0x228D0) and quality cap (0x23810), then takes the minimum.
// 1.13c D2Common+0x24610
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_24610(uint32_t pUnit) {
	if (!pUnit || W(P(pUnit), 0) != 4) return 0;
	return 0; // The table-row, item-level, and quality helpers are separate UNMAPPED rows.
}

// Find a matching saved-item row in the sorted item table. Table globals are not yet modeled.
// 1.13c D2Common+0x532C0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_532c0(uint32_t pItems, uint32_t nCount, uint32_t nKind, uint32_t nMinLevel, uint32_t nExpansion) {
	(void)pItems; (void)nCount; (void)nKind; (void)nMinLevel; (void)nExpansion;
	return 0; // Retail's indexed table base/count globals are still outside the engine model.
}

// Gate for the item-type/stat test: unit type 1, item class 0x0C, and a negative table flag.
// 1.13c D2Common+0x4FFD0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_4ffd0(uint32_t pUnit) {
	if (!pUnit || W(P(pUnit), 0) != 1 || W(P(pUnit), 0x10) != 0x0C) return 0;
	return 0; // Depends on table helper 0x33C20 and bit predicate 0x1470.
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
	(void)pUnit; (void)pItem; (void)nIncludeCurrent;
	return 0; // Inventory traversal and set-record helpers are distinct UNMAPPED rows.
}

// Unit stat accessor for the unit's data block. These unit types store the primary value at +0x10;
// other types store the value at +0x04. Retail terminates on a null block.
// 1.13c D2Common+0x302E0
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_302e0(uint32_t pUnit) {
	if (!pUnit) return 0;
	const uint32_t nType = W(P(pUnit), 0);
	const uint32_t pData = W(P(pUnit), 0x2C);
	if (!pData) return 0;
	if (nType == 2 || (nType > 3 && nType < 6))
		return static_cast<int32_t>(W(P(pData), 0x10) << 16);
	return static_cast<int32_t>(W(P(pData), 0x04));
}

// Tick inactive-room entries. The delay counter is in each record at +0x1D4; on expiry retail
// invokes 0x2DDB0 and may call the update helper 0x2DAF0. Their effects are separate rows.
// 1.13c D2Common+0x2E2C0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_2e2c0(uint32_t pLevel) {
	if (!pLevel) return;
	for (uint32_t p = W(P(pLevel), 0x47C); p; p = W(P(p), 0x1AC)) {
		if (W(P(p), 0x0C) != 0 || W(P(p), 0x10) == 0) continue;
		uint32_t nDelay = W(P(p), 0x1D4);
		W(P(p), 0x1D4, nDelay ? nDelay - 1 : 10);
	}
}

// Compute a stat value for a stat-list context. Retail first resolves a stat index (0x382B0),
// then conditionally scales the item value using the context flags and item mode.
// 1.13c D2Common+0x38B00
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_38b00(uint32_t pContext, uint32_t pItem) {
	(void)pContext; (void)pItem;
	return 0; // 0x382B0 and the stat-list register context are not modeled in this batch.
}

// Propagate a stat delta to the eight linked units selected by the stat list. Retail's two list
// lookups (0x94CE0, 0x2D9B0) and flag comparison (0x44CE0) are separate UNMAPPED rows.
// 1.13c D2Common+0x2DF80
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_2df80(uint32_t pContext, int32_t nDelta) {
	(void)pContext; (void)nDelta;
}

// Grow a UnitFinds array by fifteen entries. The Fog allocator is unresolved, so leave the record
// untouched rather than publishing a larger capacity without backing storage.
// 1.13c D2Common+0x120F0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_120f0(uint32_t pFinds) {
	(void)pFinds;
}

// Rectangle membership predicate over the room bounds returned by 0x3BA60; its coordinate and
// room arguments arrive in retail registers, so the bounds provider must be surfaced by its own row.
// 1.13c D2Common+0x121D0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_121d0(uint32_t pRoom, int32_t x, int32_t y, int32_t nMargin) {
	(void)pRoom; (void)x; (void)y; (void)nMargin;
	return 0; // Depends on the room-bounds helper 0x3BA60.
}
