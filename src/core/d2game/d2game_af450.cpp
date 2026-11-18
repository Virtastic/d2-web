#include "../d2moo/compat/test_seam.h"
#include "d2game_af450.h"
#include "d2game_retail_rw.h"
uint32_t D2R_AF450_Room(uint32_t) { __builtin_trap(); }
uint32_t D2R_AF450_Area(uint32_t) { __builtin_trap(); }
uint32_t D2R_AF450_B10459(uint32_t) { __builtin_trap(); }
uint32_t D2R_AF450_C10585(uint32_t) { __builtin_trap(); }
uint32_t D2R_AF450_S10057(uint32_t) { __builtin_trap(); }
void     D2R_AF450_386F0(uint32_t) { __builtin_trap(); }
uint32_t D2R_AF450_1130(uint32_t) { __builtin_trap(); }
uint32_t D2R_AF450_1100(uint32_t) { __builtin_trap(); }
uint32_t D2R_AF450_A24C0(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t*) { __builtin_trap(); }
void     D2R_AF450_DFFD0(uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
void     D2R_AF450_37480(uint32_t, uint32_t) { __builtin_trap(); }
void     D2R_AF450_2E4F0(uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_AF450_E1570(uint32_t, uint32_t) { __builtin_trap(); }
void     D2R_AF450_5E3C0(uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_AF450_A4C0(uint32_t) { __builtin_trap(); }
void     D2R_AF450_8C3B0(uint32_t, uint32_t, uint32_t, uint32_t) { __builtin_trap(); }

// 1.13c D2Game+0xAF450 -- ECX = game, EDX = unit, five unused stack words (ret 0x14). A unit of type 0 only (the tail's [unit] != 0
// branch is dead because the unit type was tested at entry).
uint32_t D2Retail113_AF450(uint32_t game, uint32_t unit, uint32_t edi)
{
	if (unit == 0 || rw32(unit, 0) != 0) return 0;
	ww32(unit, 0xc4, rw32(unit, 0xc4) | 0x40);
	const uint32_t room = D2_CALL(D2R_AF450_Room, unit);
	const uint32_t area = D2_CALL(D2R_AF450_Area, room);
	const uint32_t b = D2_CALL(D2R_AF450_B10459, area) & 0xff;
	const uint32_t c = D2_CALL(D2R_AF450_C10585, b);
	if (D2_CALL(D2R_AF450_S10057, room) != 0 || D2_CALL(D2R_AF450_Area, room) == 0x88) {
		D2_CALL(D2R_AF450_DFFD0, unit, 0x18u, unit);
		return 0;
	}
	D2_CALL(D2R_AF450_386F0, unit);
	uint32_t out = 0;
	const uint32_t r1130 = D2_CALL(D2R_AF450_1130, unit);
	const uint32_t r1100 = D2_CALL(D2R_AF450_1100, unit);
	const uint32_t res = D2_CALL(D2R_AF450_A24C0, game, unit, room, r1100, r1130, c, &out);
	D2_CALL(D2R_AF450_DFFD0, unit, 7u, 0u);
	if (res != 0) {
		D2_CALL(D2R_AF450_37480, unit, out == 0 ? 0xffffffffu : rw32(out, 0xc));
		if (out != 0) {
			D2_CALL(D2R_AF450_2E4F0, out, rw32(unit, 0xc));
			const uint32_t e = D2_CALL(D2R_AF450_E1570, game, out);
			if (e != 0) {
				D2_CALL(D2R_AF450_2E4F0, e, rw32(unit, 0xc));
				D2_CALL(D2R_AF450_5E3C0, e, game);
			}
			D2_CALL(D2R_AF450_5E3C0, out, game);
		}
	}
	if (edi != 0) {
		const uint32_t id = rw32(edi, 0xc), t = rw32(edi, 0);
		D2_CALL(D2R_AF450_8C3B0, 0x7cu, t & 0xff, id, rw32(D2_CALL(D2R_AF450_A4C0, unit), 0x9c));
		return out;
	}
	return res;
}
