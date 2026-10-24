// D2Common 1.13c batch: 0x5A8A0 (MonStats walk/run anim speeds) and 0x24F90 (socket chain depth).
#include <cstdint>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Items.h"
#include "D2Inventory.h"
#include "DataTbls/AnimTbls.h"
#include "Units/Units.h"

// 1.13c D2Common+0x5A8A0: per MonStats row, walk speed = AnimData(base,WALK).speed scaled by
// velocity/base velocity, run speed = (base walk)/2 for rows < 410 else AnimData(base,RUN).speed,
// scaled by run/base run; both clamped to [0,32767]. Same loop DATATBLS_LoadMonStatsTxt carries.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_5a8a0(int nCount)
{
	D2MonStatsTxt* t = sgptDataTables->pMonStatsTxt;
	for (int i = 0; i < nCount; ++i)
	{
		int b = t[i].nBaseId;
		if (b < 0 || b >= nCount) { t[i].nBaseId = int16_t(i); b = i; }
		int w = int(DATATBLS_GetAnimDataRecord(0, b, MONMODE_WALK, UNIT_MONSTER, 0)->dwAnimSpeed);
		if (b != i && t[b].nVelocity > 0) w = int(uint32_t(int(t[i].nVelocity) * uint32_t(w)) / uint32_t(int(t[b].nVelocity)));  // retail scales in unsigned 32-bit
		w = w < 1 ? 0 : (w > 0x7ffe ? 0x7fff : w);
		t[i].nWalkAnimSpeed = int16_t(w);
		int r = i < 410 ? int(t[b].nWalkAnimSpeed) / 2
		                : int(DATATBLS_GetAnimDataRecord(0, b, MONMODE_RUN, UNIT_MONSTER, 0)->dwAnimSpeed);
		if (b != i && t[b].nRun > 0) r = int(uint32_t(int(t[i].nRun) * uint32_t(r)) / uint32_t(int(t[b].nRun)));
		t[i].nRunAnimSpeed = int16_t(r < 1 ? 0 : (r > 0x7ffe ? 0x7fff : r));
	}
}

// 1.13c D2Common+0x24F90 (unit in EAX): for an item with a valid inventory (signature 0x1020304),
// count links of the chain starting at inventory+0x0C (item -> itemdata+0x14 ... next at
// itemdata+0x64); stops at a non-item, null data, or the -0x5C sentinel. 0 if not socketable.
EMSCRIPTEN_KEEPALIVE int d2_fn_d2common_24f90(uint32_t unit)
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
