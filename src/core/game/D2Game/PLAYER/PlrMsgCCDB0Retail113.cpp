#include <test_seam.h>
#include <D2Game/PLAYER/PlrMsgRetail113.h>
#include <unistd.h>
uint32_t D2R_CCDB0_AB26(uint32_t, uint32_t) { __builtin_trap(); }

// 1.13c D2Game+0xCCDB0 (ECX = unit id, EAX = game, stack = record): true unless the id names a state-2 entry of the game's second table (+0x1720) that
// Ordinal 10391 finds on the record's unit (+0x60).
bool D2Retail113_CCDB0(uint32_t thisId, uint32_t game, uint32_t param1)
{
	if (!game) _exit(-1);
	const uint32_t n = d2g_chain(game, 0x1720, thisId);
	if (!n) return true;
	if (rw32(n, 0x10) != 2) return true;
	return D2_CALL(D2R_CCDB0_AB26, rw32(param1, 0x60), n) == 0;
}
