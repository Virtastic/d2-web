// 1.13c D2Common implementation batch: +0x23D70.
// Set-item lookup: for a set-quality item unit, resolve the item's file index through the 0x22640
// seam and return the setitems.txt row's nSetItems at +0x2E. Anything else yields -1.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"

static_assert(offsetof(D2SetItemsTxt, nSetItems) == 0x2E, "setitems.txt nSetItems");

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
	inline int16_t RdI16(const uint8_t* b, int o) { return *reinterpret_cast<const int16_t*>(b + o); }
}

// 0x22640 seam: item unit -> item data file index (unit type 4, +0x14 data, +0x28 file index).
struct D2Common23D70Env
{
	int32_t (*item_file_index)(uint32_t pUnit);
};

// 1.13c D2Common+0x23D70 (#10550): setitems.txt nSetItems for a set item, else -1.
// The unit must be type 4 (item) and its item data quality must be 5 (set).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t ITEMS_GetSetItemNSetItems(uint32_t pUnit, const D2Common23D70Env* pEnv)
{
	if (!pUnit)
		return -1;

	uint8_t* unit = P(pUnit);
	if (W(unit, 0x00) != 4)
		return -1;
	const uint32_t pItemData = W(unit, 0x14);
	if (!pItemData || W(P(pItemData), 0x00) != 5)
		return -1;

	if (!pEnv || !pEnv->item_file_index)
		return -1;
	const int32_t nFileIndex = pEnv->item_file_index(pUnit);
	if (nFileIndex < 0 || nFileIndex >= sgptDataTables->nSetItemsTxtRecordCount)
		return -1;
	if (!sgptDataTables->pSetItemsTxt)
		return -1;

	const uint8_t* pRow = reinterpret_cast<const uint8_t*>(sgptDataTables->pSetItemsTxt)
	                    + static_cast<size_t>(nFileIndex) * sizeof(D2SetItemsTxt);
	return RdI16(pRow, 0x2E);
}
