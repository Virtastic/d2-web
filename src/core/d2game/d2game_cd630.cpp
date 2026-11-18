#include "../compat/test_seam.h"
#include "d2game_cc750.h"
// 1.13c D2Game+0xCD630 (ECX = packet length, EAX = packet, EBX = out B; stack game, unit, out A): both outs are cleared; a length other than 9 gives 3; the
// packet's unit type (+1) of 6 or more gives 2; otherwise the target check CCFE0(type, game, id, unit) answers and, when it is 0, the type and id are stored.
int32_t D2Retail113_CD630(uint32_t len, uint32_t pkt, uint32_t game, uint32_t unit, uint32_t outA, uint32_t outB)
{
	ww32(outA, 0, 0); ww32(outB, 0, 0);
	if (len != 9) return 3;
	const uint32_t a = rw32(pkt, 1), b = rw32(pkt, 5);
	if (5 < a) return 2;
	const int32_t r = (int32_t)D2Retail113_CCFE0((int32_t)a, game, b, unit);
	if (r == 0) { ww32(outA, 0, a); ww32(outB, 0, b); }
	return r;
}
