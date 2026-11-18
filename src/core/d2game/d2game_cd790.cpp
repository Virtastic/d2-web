#include "../compat/test_seam.h"
#include "d2game_cc750.h"
uint32_t D2R_CD790_A478(uint32_t) { __builtin_trap(); }
uint32_t D2R_CD790_1280() { __builtin_trap(); }
uint32_t D2R_CD790_A68E(uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_CD790_AB56(uint32_t) { __builtin_trap(); }
uint32_t D2R_CD790_AB5C(uint32_t) { __builtin_trap(); }
uint32_t D2R_CD790_AB4A(uint32_t) { __builtin_trap(); }
void     D2R_CD790_E07A0(uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_CD790_A598(uint32_t) { __builtin_trap(); }
uint32_t D2R_CD790_AAC6(uint32_t) { __builtin_trap(); }
uint32_t D2R_CD790_AAD8(uint32_t) { __builtin_trap(); }
void     D2R_CD790_AB08(uint32_t, uint32_t) { __builtin_trap(); }
void     D2R_CD790_AAFC(uint32_t, uint32_t) { __builtin_trap(); }
void     D2R_CD790_AB14(uint32_t, uint32_t) { __builtin_trap(); }
void     D2R_CD790_AB2C(uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_CD790_AAF0(uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_CD790_AAAE(uint32_t) { __builtin_trap(); }
uint32_t D2R_CD790_AB32(uint32_t) { __builtin_trap(); }
void     D2R_CD790_A5FE(uint32_t, uint32_t) { __builtin_trap(); }

// 1.13c D2Game+0xCD790 (ECX = this compared with 0xAAAE, EAX = EDI compared with 0xAB32, stack param_1, unit; ret 8). Rewritten from the asm: the three
// setters (0x409 / 0xf / 0x4d) are restored from the getters read before them, and the last call is 0xA5FE(d, S4) when 0xA598 returned the unit itself,
// else 0xAB2C(d, X5, X6). The byte getter's dword keeps stack residue above bit 7 (the capture masks it: hookmask 0xab14 0x1f).
uint32_t D2Retail113_CD790(uint32_t thisp, uint32_t eax, uint32_t param1, uint32_t unit)
{
	D2_CALL(D2R_CD790_A478, unit);
	if (!D2_CALL(D2R_CD790_1280)) return 0;
	if (D2_CALL(D2R_CD790_A68E, unit, 0x36u) || D2_CALL(D2R_CD790_A68E, unit, 0x6cu)) return 0;
	const uint32_t d = rw32(unit, 0x2c);
	const uint32_t x1 = D2_CALL(D2R_CD790_AB56, d);
	const uint32_t x2 = D2_CALL(D2R_CD790_AB5C, d);
	const uint32_t x3 = D2_CALL(D2R_CD790_AB4A, d);
	D2_CALL(D2R_CD790_E07A0, unit, param1, eax);
	const uint32_t y = D2_CALL(D2R_CD790_A598, rw32(unit, 0x2c));
	const uint32_t s4 = (y == unit) ? 0 : y;   // sete; dec; and: kept only when it is not the unit itself
	const uint32_t x5 = D2_CALL(D2R_CD790_AAC6, d);
	const uint32_t x6 = D2_CALL(D2R_CD790_AAD8, d);
	D2_CALL(D2R_CD790_AB08, d, 0x409u);
	D2_CALL(D2R_CD790_AAFC, d, 0xfu);
	D2_CALL(D2R_CD790_AB14, d, 0x4du);
	D2_CALL(D2R_CD790_AB2C, d, thisp, eax);
	uint32_t r = 0;
	if (D2_CALL(D2R_CD790_AAF0, d, unit, 0u)) {
		if (thisp == D2_CALL(D2R_CD790_AAAE, d) && eax == D2_CALL(D2R_CD790_AB32, d)) r = 1;
	}
	D2_CALL(D2R_CD790_AB08, d, x1);
	D2_CALL(D2R_CD790_AAFC, d, x2);
	D2_CALL(D2R_CD790_AB14, d, x3);
	if (s4) D2_CALL(D2R_CD790_A5FE, d, s4);
	else D2_CALL(D2R_CD790_AB2C, d, x5, x6);
	return r;
}
