#include "../compat/test_seam.h"
#include "d2game_cc750.h"
uint32_t D2R_CCFE0_E03A0(uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_CCFE0_E150(uint32_t) { __builtin_trap(); }
uint32_t D2R_CCFE0_A478(uint32_t) { __builtin_trap(); }
uint32_t D2R_CCFE0_A556(uint32_t) { __builtin_trap(); }
uint32_t D2R_CCFE0_A56E(uint32_t) { __builtin_trap(); }
uint32_t D2R_CCFE0_A454(uint32_t) { __builtin_trap(); }
uint32_t D2R_CCFE0_A3B8(uint32_t) { __builtin_trap(); }
uint32_t D2R_CCFE0_CB690(uint32_t, uint32_t) { __builtin_trap(); }

// 1.13c D2Game+0xCCFE0 (EAX = mode, ECX = game, EDX = id, stack = unit): the target must exist (else 1), mode 4 with the E150 test true gives 0, the two
// units' act bytes (Ordinal chain A478 -> A556 -> A56E, signed byte) must match (else 2); then CB690 compares the two units' level words, each pushed whole:
// types 2, 4 and 5 read them at +0xC / +0x10 of the unit's data (+0x2C), any other type asks A454 / A3B8 (0 for no data).
uint32_t D2Retail113_CCFE0(int32_t mode, uint32_t game, uint32_t id, uint32_t unit)
{
	const uint32_t o = D2_CALL(D2R_CCFE0_E03A0, game, id);
	if (!o) return 1;
	if (mode == 4 && D2_CALL(D2R_CCFE0_E150, unit)) return 0;
	const int8_t a = (int8_t)D2_CALL(D2R_CCFE0_A56E, D2_CALL(D2R_CCFE0_A556, D2_CALL(D2R_CCFE0_A478, unit)));
	const int8_t b = (int8_t)D2_CALL(D2R_CCFE0_A56E, D2_CALL(D2R_CCFE0_A556, D2_CALL(D2R_CCFE0_A478, o)));
	if (a != b) return 2;
	const int32_t t = (int32_t)rw32(o, 0);
	const uint32_t d = rw32(o, 0x2c);
	const bool typed = t == 2 || (3 < t && t < 6);
	const uint32_t u3 = typed ? rw32(d, 0xc) : (d ? D2_CALL(D2R_CCFE0_A454, d) : 0);
	if (typed) return D2_CALL(D2R_CCFE0_CB690, u3, rw32(d, 0x10));
	return D2_CALL(D2R_CCFE0_CB690, u3, d ? D2_CALL(D2R_CCFE0_A3B8, d) : 0u);
}
