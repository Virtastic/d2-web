// ECX = game, EDX = player; ret 4. Act 5 quest-record start flags: the q32::questStartFlags shape over record 0x26 (states 0, 0xF, 0xD, 0xE),
// firing the quest-start notifier with quest number 0x22. Returns 0.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x590C0
uint32_t d2game_590C0_q32(const Call& X, uint32_t game, uint32_t player) { return questStartFlags(X, game, player, 0x26, 0x22); }
