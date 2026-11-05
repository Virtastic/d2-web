// D2Common 1.13c batch: 0x24F90 (socket chain depth).
#include <cstdint>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Items.h"
#include "D2Inventory.h"
#include "DataTbls/AnimTbls.h"
#include "Units/Units.h"

// +0x5A8A0 moved to d2common_5a8a0.cpp (injectable animdata lookup, FN probe).

// 1.13c D2Common+0x24F90 (unit in EAX): for an item with a valid inventory (signature 0x1020304),
// count links of the chain starting at inventory+0x0C (item -> itemdata+0x14 ... next at
// itemdata+0x64); stops at a non-item, null data, or the -0x5C sentinel. 0 if not socketable.
EMSCRIPTEN_KEEPALIVE int D2Common_24F90_SocketChainCount(uint32_t unit)
{
	auto w = [](uint32_t a, int o) { return *reinterpret_cast<uint32_t*>(uintptr_t(a) + o); };
	if (!unit || w(unit, 0) != 4 || !ITEMS_CheckIfSocketableByItemId(int(w(unit, 4)))) return 0;
	uint32_t inv = w(unit, 0x60);
	if (!inv || w(inv, 0) != 0x1020304) return 0;
	uint32_t it = w(inv, 0xC);
	if (!it) return 0;
	int n = 0;
	do {
		++n;
		if (w(it, 0) != 4) return n;
		uint32_t d = w(it, 0x14);
		if (!d || d == uint32_t(-0x5c)) return n;
		it = w(d, 100);
	} while (it);
	return n;
}
