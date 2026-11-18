#include <unistd.h>
#include "../compat/test_seam.h"
#include "d2game_b1820.h"
#include "d2game_retail_rw.h"
uint32_t D2R_B1820_48560(uint32_t) { __builtin_trap(); }
uint32_t D2R_B1820_B0A20(uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_B1820_AD78(uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_B1820_A514(uint32_t) { __builtin_trap(); }
uint32_t D2R_B1820_1450(uint32_t, uint32_t) { __builtin_trap(); }
void     D2R_B1820_E0460(uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_B1820_1130(uint32_t) { __builtin_trap(); }
uint32_t D2R_B1820_1100(uint32_t) { __builtin_trap(); }
uint32_t D2R_B1820_A478(uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_B1820_AD66(uint32_t) { __builtin_trap(); }
void     D2R_B1820_B17B0(uint32_t, uint32_t*, uint32_t, uint32_t, uint32_t, int) { __builtin_trap(); }
uint32_t D2R_B1820_DFA90(uint32_t) { __builtin_trap(); }

// 1.13c D2Game+0xB1820 (EAX = unit; ret 0x14). param_1 is the game (the three node tables at +0x1120/+0x1320/+0x1920, 128 buckets each,
// chained through +0xE4, key +0xC), p3 a flag. [[unit+0x14]+0x38] is the mode: 1/2/4 look the unit's id (0x48560) up in their table,
// 3 asks B17B0 for a target. Returns 1 with *param_4 = node and *param_5 = value, else 0 (clearing +0x34/+0x38 of the unit's data on the
// way out). A null game with mode 1/2/4 is retail's fatal path.
uint32_t D2Retail113_B1820(uint32_t unit, uint32_t game, uint32_t param_2, uint32_t param_3, uint32_t* param_4, int32_t* param_5)
{
	if (unit == 0 || rw32(unit, 0) != 1 || rw32(unit, 0x14) == 0) return 0;
	const uint32_t mode = rw32(rw32(unit, 0x14), 0x38);
	if (mode == 0) return 0;
	uint32_t node = 0, val = 0;
	bool have = false;
	if (mode - 1 <= 3) {
		if (mode == 3) {
			const uint32_t ebx = D2_CALL(D2R_B1820_A514, unit);
			const uint32_t eax = D2_CALL(D2R_B1820_1450, unit + 0x20, 2u);
			uint32_t bl = 0;
			bool call = false;
			if (ebx == 1) { call = true; bl = (eax == 0) ? 0 : 2; }
			else if (eax != 0) {
				if (ebx == 0) { call = true; bl = 2; }
				else if (ebx == 2) { call = true; bl = 0; }
			}
			if (call) D2_CALL(D2R_B1820_E0460, unit, bl, 1u);
			uint32_t blk[7] = { 0, 0x7fffffff, 0, 0, 0, 0, 0 };
			if (param_3 == 0) {
				const uint32_t r1130 = D2_CALL(D2R_B1820_1130, unit);
				const uint32_t r1100 = D2_CALL(D2R_B1820_1100, unit);
				const uint32_t ad = D2_CALL(D2R_B1820_AD66, D2_CALL(D2R_B1820_A478, unit, r1100, r1130));
				blk[2] = param_2; blk[3] = 0x23; blk[4] = ad; blk[5] = 0; blk[6] = 0x7fffffff;
				D2_CALL(D2R_B1820_B17B0, 5u, blk, game, unit, 0u, 7);
			} else {
				blk[3] = 0x7fffffff;
				D2_CALL(D2R_B1820_B17B0, 6u, blk, game, unit, 0u, 4);
			}
			node = blk[0]; val = blk[1];
			D2_CALL(D2R_B1820_E0460, unit, ebx, 1u);   // BL is reloaded from the saved A514 result
			have = true;
		} else {
			const uint32_t id = D2_CALL(D2R_B1820_48560, unit);
			const uint32_t base = mode == 1 ? 0x1120 : mode == 2 ? 0x1320 : 0x1920;
			if (game == 0) _exit(-1);
			uint32_t n = rw32(game, base + (id & 0x7f) * 4);
			while (n != 0 && rw32(n, 0xc) != id) n = rw32(n, 0xe4);
			if (n != 0) {
				node = n;
				val = D2_CALL(D2R_B1820_B0A20, n, unit);
				have = !(param_3 != 0 && D2_CALL(D2R_B1820_AD78, n, unit, 4u) != 0);
			}
		}
	}
	if (have && node != 0) {
		const int32_t k = (int32_t)rw32(node, 0);
		if (k >= 0) {
			bool ok = false;
			if (k <= 1) ok = D2_CALL(D2R_B1820_DFA90, node) == 0;
			else if (k == 3) ok = true;
			if (ok) { *param_4 = node; *param_5 = (int32_t)val; return 1; }
		}
	}
	ww32(rw32(unit, 0x14), 0x38, 0);
	ww32(rw32(unit, 0x14), 0x34, 0);
	return 0;
}
