#include "../d2moo/compat/test_seam.h"
#include "d2game_b1660.h"
#include "d2game_retail_rw.h"
uint32_t D2R_B1660_Ord(uint32_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t) { __builtin_trap(); }
uint32_t D2R_B1660_DFA90(uint32_t) { __builtin_trap(); }
uint32_t D2R_B1660_E0A60(uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_B1660_218E0() { __builtin_trap(); }
uint32_t D2R_B1660_21130(uint32_t) { __builtin_trap(); }
uint32_t D2R_B1660_21100(uint32_t) { __builtin_trap(); }
int32_t  D2R_B1660_B0A20(uint32_t) { __builtin_trap(); }
uint32_t D2R_B1660_B08A0(uint32_t) { __builtin_trap(); }

// 1.13c D2Game+0xB1660
uint32_t D2Retail113_B1660(uint32_t unused, uint32_t p2, uint32_t unit, uint32_t p4)
{
	if (!unit) return 0;
	if (!((int32_t)rw32(unit, 0) < 0 || rw32(unit, 0) > 1 || D2_CALL(D2R_B1660_DFA90, unit) != 0 ||
	      !((rw32(unit, 0xC4) >> 2) & 1) || D2_CALL(D2R_B1660_E0A60, p2, unused) == 0)) {
		const int32_t v = D2_CALL(D2R_B1660_B0A20, p2);
		if (v <= (int32_t)rw32(p4, 12)) {
			const uint32_t b = D2_CALL(D2R_B1660_B08A0, unit);   // the full EAX: the callee returns a byte and the decompile reads CONCAT31(stale, al)
			if (b < 2) { if ((int32_t)rw32(p4, 24) <= v) return 0; }
			else if ((int32_t)rw32(p4, 4) <= v) return 0;
			if (rw32(p4, 8) == 0 || D2_CALL(D2R_B1660_Ord, 10839u, (uintptr_t)unit, (uintptr_t)p2, 4, 0, 0) == 0) {
				if (b > 1) { ww32(p4, 0, unit); ww32(p4, 4, (uint32_t)v); return 0; }
				ww32(p4, 20, unit); ww32(p4, 24, (uint32_t)v);
			}
		}
		return 0;
	}
	if (rw32(unit, 0) == 1 && (p2 == 0 || rw32(p2, 0) != 0) && D2_CALL(D2R_B1660_Ord, 10830u, (uintptr_t)p2, 0, 0, 0, 0) == 0 &&
	    rw32(p4, 8) != 0 && D2_CALL(D2R_B1660_Ord, 10648u, (uintptr_t)unit, 0, 0, 0, 0) == 0) {
		const uint32_t rec = D2_CALL(D2R_B1660_218E0);
		if (rw32(rec, 8) & 8) {
			const uint32_t a = D2_CALL(D2R_B1660_21130, unit), b = D2_CALL(D2R_B1660_21100, unit);
			const uint32_t u = D2_CALL(D2R_B1660_Ord, 10331u, (uintptr_t)unit, (uintptr_t)b, (uintptr_t)a, 0, 0);
			if (rw32(p4, 16) == D2_CALL(D2R_B1660_Ord, 10916u, (uintptr_t)u, 0, 0, 0, 0)) {
				ww16(rec, 8, (uint16_t)(rw16(rec, 8) | 8));
				ww32(p4, 8, 0);
			}
		}
	}
	return 0;
}
