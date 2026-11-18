#include "../compat/test_seam.h"
#include "d2game_cc750.h"
#include <unistd.h>
uint32_t D2R_CCE20_AB26(uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2R_CCE20_A6A6(uint32_t) { __builtin_trap(); }

// 1.13c D2Game+0xCCE20 (ECX = unit id, EAX = game, EDI = record): true when the id is not in the second table (+0x1720); when it is, Ordinal 10391 finding it
// on the record's unit (+0x60) gives "the unit's own (Ordinal 11017) entry differs"; not found gives false.
bool D2Retail113_CCE20(uint32_t id, uint32_t game, uint32_t edi)
{
	if (!game) _exit(-1);
	const uint32_t n = d2g_chain(game, 0x1720, id);
	if (!n) return true;
	if (D2_CALL(D2R_CCE20_AB26, rw32(edi, 0x60), n) == 0)
		return D2_CALL(D2R_CCE20_A6A6, rw32(edi, 0x60)) != n;
	return false;
}
