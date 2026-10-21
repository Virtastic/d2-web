// D2Common 1.13c batch: 0x1B1D0 (#11136). Per-unit stat scaling for a monster unit (type 1) at
// difficulty param_3 (1 or 2; 0 = normal is a no-op). Callees are weak seams: 0x12A0 as the MonStats
// record pointer for an id (arg is register-only in the decompile; unit+4 class id assumed), 0x38B70 the unit stat getter, 0x3A740 the stat setter (STATLIST_SetUnitStat).
#include <cstdint>
#include <emscripten.h>

int DATATBLS_ApplyRatio(int nValue, int nMultiplier, int nDivisor);

extern "C" {
__attribute__((weak)) uint32_t d2_sub_12a0_record(int32_t id);
__attribute__((weak)) int32_t d2_sub_38b70_get(uint32_t pUnit, int32_t nStat);
__attribute__((weak)) void d2_sub_3a740_set(uint32_t pUnit, int32_t nStat, int32_t nValue);
// retail DAT_6fde4e3c: 3 rows x 5 dwords {statId, ?, ratioNightmare, ?, ratioHell}
__attribute__((weak)) uint32_t d2common_data_4e3c[15];

// 1.13c D2Common+0x1B1D0
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_1b1d0(uint32_t pUnit, int32_t nArg2, uint8_t nDiff) {
	if (nArg2 != 0 || nDiff == 0 || pUnit == 0) return;
	if (*reinterpret_cast<int32_t*>((intptr_t)pUnit) != 1) return;
	const uint32_t rec = d2_sub_12a0_record(*reinterpret_cast<int32_t*>((intptr_t)pUnit + 4));
	if (!rec) return;
	const uint8_t* r = reinterpret_cast<const uint8_t*>((intptr_t)rec);
	if (r[0x4C] == 1) return;
	for (int i = 0; i < 3; ++i) {
		const uint32_t* row = d2common_data_4e3c + i * 5;
		const int32_t stat = static_cast<int32_t>(row[0]);
		const int32_t v = d2_sub_38b70_get(pUnit, stat);
		d2_sub_3a740_set(pUnit, stat, DATATBLS_ApplyRatio(static_cast<int32_t>(nDiff == 1 ? row[2] : row[4]), v, 100));
	}
	d2_sub_3a740_set(pUnit, 0xC, *reinterpret_cast<const int16_t*>(r + 0xAA) + nDiff * 0x19);
}
}
