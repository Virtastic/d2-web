#include "../d2moo/compat/test_seam.h"
#include "d2game_2aae0.h"
#include "d2game_retail_rw.h"
void     D2R_2AAE0_Enter(uint32_t) { __builtin_trap(); }
void     D2R_2AAE0_Leave(uint32_t) { __builtin_trap(); }
uint32_t D2R_2AAE0_Find(uint32_t, uint32_t) { __builtin_trap(); }
void     D2R_2AAE0_Lock(uint32_t, uint32_t) { __builtin_trap(); }
void     D2R_2AAE0_Fatal(uint32_t) { __builtin_trap(); }

// 1.13c D2Game+0x2AAE0 (stdcall, game guid): finds a game by guid under the game table's critical section (table+0x50) and hands it back
// with the game's own lock (+0x18) taken; the table lock is left again before returning. A null table or unknown guid gives null; a game
// whose lock pointer is null is the 0x112E assert.
uint32_t D2Retail113_2AAE0(uint32_t table, uint32_t guid)
{
	if (table == 0) return 0;
	D2_CALL(D2R_2AAE0_Enter, table + 0x50);
	const uint32_t game = D2_CALL(D2R_2AAE0_Find, table, guid);
	if (game == 0) { D2_CALL(D2R_2AAE0_Leave, table + 0x50); return 0; }
	if (rw32(game, 0x18) == 0) D2_CALL(D2R_2AAE0_Fatal, 0x112Eu);
	D2_CALL(D2R_2AAE0_Lock, rw32(game, 0x18), 0x1141u);
	D2_CALL(D2R_2AAE0_Leave, table + 0x50);
	return game;
}
