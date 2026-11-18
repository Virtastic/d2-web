#include "../compat/test_seam.h"
#include "d2game_b6720.h"
#include "d2game_retail_rw.h"
uint32_t D2R_B6720_1320(uint32_t) { __builtin_trap(); }
void     D2R_B6720_8E6F0(uint32_t, uint32_t) { __builtin_trap(); }
void     D2R_B6720_8EA80(uint32_t, uint32_t, uint32_t, uint32_t) { __builtin_trap(); }

// 1.13c D2Game+0xB6720 (EAX = game, EBX = unit): common tail of B6840/B6A60. A player's class record (+0x108) picks the frame count n
// (0xB otherwise); every attached node of kind 0 is released through 0x8E6F0; then 0x8EA80(game, unit, 1, game+0xA8 + n). Returns 1.
uint32_t D2Retail113_B6720(uint32_t game, uint32_t unit)
{
	uint32_t n = 0xb;
	if (unit) {
		if (rw32(unit, 0) == 1) {
			const uint32_t p = D2_CALL(D2R_B6720_1320, rw32(unit, 4));
			if (p) n = *(const uint8_t*)(uintptr_t)(p + 0x108);
		}
		for (uint32_t q = rw32(unit, 0xdc); q; ) {
			const uint32_t cur = q;
			const uint8_t c = *(const uint8_t*)(uintptr_t)q;
			q = rw32(q, 0x24);
			if (c == 0) D2_CALL(D2R_B6720_8E6F0, cur, game);
		}
	}
	D2_CALL(D2R_B6720_8EA80, game, unit, 1u, rw32(game, 0xa8) + n);
	return 1;
}
