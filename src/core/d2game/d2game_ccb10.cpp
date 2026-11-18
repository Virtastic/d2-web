#include "../compat/test_seam.h"
#include "d2game_cc750.h"
uint32_t D2R_CCB10_E03A0(uint32_t, uint32_t) { __builtin_trap(); }

// 1.13c D2Game+0xCCB10 (this = record, then game, param_2): 1 when the record is flagged (+0x6C), has no owner word (+0x68) and its unit (+100) is live and
// is not param_2; otherwise 0.
uint32_t D2Retail113_CCB10(uint32_t thisp, uint32_t game, uint32_t param2)
{
	if (*(const uint8_t*)(uintptr_t)(thisp + 0x6c) != 0 && rw32(thisp, 0x68) == 0) {
		const uint32_t r = D2_CALL(D2R_CCB10_E03A0, game, rw32(thisp, 100));
		if (r && r != param2) return 1;
	}
	return 0;
}
