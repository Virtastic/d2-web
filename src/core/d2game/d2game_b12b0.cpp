#include "../d2moo/compat/test_seam.h"
#include "d2game_b12b0.h"
#include "d2game_retail_rw.h"
uint32_t D2R_B12B0_Ord(uint32_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t) { __builtin_trap(); }
uint32_t D2R_B12B0_B0A20(uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_B12B0_21260(uint32_t) { __builtin_trap(); }

// 1.13c D2Game+0xB12B0: the distance comes first; p4 = {unit*, count, dist, maxDist}; EBX = EDX (retail does mov ebx,edx).
uint32_t D2Retail113_B12B0(uint32_t, uint32_t p2, uint32_t u, uint32_t p4)
{
	const int32_t d = (int32_t)D2_CALL(D2R_B12B0_B0A20, p2, u);
	if (d <= (int32_t)rw32(p4, 12) && u && rw32(u, 0) == 1 && rw32(u, 0x10) == 0xc && ((rw32(u, 0xc4) >> 1) & 1)) {
		if (D2_CALL(D2R_B12B0_Ord, 10541u, (uintptr_t)u, 0, 0, 0, 0) == 0 && D2_CALL(D2R_B12B0_21260, u) == p2) {
			ww32(p4, 4, rw32(p4, 4) + 1);
			ww32(p4, 0, u);
			ww32(p4, 8, (uint32_t)d);
		}
	}
	return 0;
}
