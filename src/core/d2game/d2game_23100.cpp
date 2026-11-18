#include "../d2moo/compat/test_seam.h"
#include "d2game_23100.h"
#include "d2game_retail_rw.h"
uint32_t D2R_23100_2790(uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_23100_1560(uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_23100_15D0(uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_23100_Tables() { __builtin_trap(); }
uint32_t D2R_23100_BitMask1() { __builtin_trap(); }

// 1.13c D2Game+0x23100 -- unique-mod row eligibility (callee of the 0x23980 and 0x23AE0 rolls). Gates, then the exclude walk (it stops at the
// first non-positive id; 0x2790 gets EAX = unit, EDI = id), then the pick kind: 1 needs 0x1560(class, 4), 2 refuses MonStats rows whose +0xC
// byte hits gdwBitMasks[1] and any 0x15D0(class, 0x11) hit, 3 needs 0x1560(class, 2). A null unit passes class -1.
uint32_t D2Retail113_23100(uint32_t unit, uint32_t entry, int32_t force)
{
	if (*(const uint8_t*)(uintptr_t)(entry + 6) == 0 || (force == 0 && rw16(entry, 4) > 99)) return 0;
	for (uint32_t i = 0; i < 2; ++i) {
		const int16_t id = (int16_t)rw16(entry, 10 + 2 * i);
		if (id < 1) break;
		if (D2_CALL(D2R_23100_2790, unit, (uint32_t)(int32_t)id) != 0) return 0;
	}
	const uint8_t kind = *(const uint8_t*)(uintptr_t)(entry + 9);
	const uint32_t cls = unit ? rw32(unit, 4) : 0xffffffffu;
	if (kind == 1) {
		if (D2_CALL(D2R_23100_1560, cls, 4u) == 0) return 0;
	} else if (kind == 2) {
		if (unit && (int32_t)cls >= 0) {
			const uint32_t dt = D2_CALL(D2R_23100_Tables);
			if ((int32_t)cls < (int32_t)rw32(dt, 0xa80)) {
				const uint32_t row = cls * 0x1a8 + rw32(dt, 0xa78);
				if (row != 0 && ((uint32_t)*(const uint8_t*)(uintptr_t)(row + 0xc) & D2_CALL(D2R_23100_BitMask1)) != 0) return 0;
			}
		}
		if (D2_CALL(D2R_23100_15D0, cls, 0x11u) != 0) return 0;
	} else if (kind == 3) {
		if (D2_CALL(D2R_23100_1560, cls, 2u) == 0) return 0;
	}
	return 1;
}
