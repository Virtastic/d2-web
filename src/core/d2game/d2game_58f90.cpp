// ECX = game, EDX = player; ret 4. Act 5 quest-record start flags: the q32::questStartFlags shape over record 0x27 (states 0, 0xF, 0xD, 0xE),
// firing the quest-start notifier with quest number 0x23. Returns 0.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x58F90
uint32_t d2game_58F90_q32(const Call& X, uint32_t game, uint32_t player) { return questStartFlags(X, game, player, 0x27, 0x23); }
