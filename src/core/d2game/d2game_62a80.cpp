// 0x62A80 (ECX = game, EDX = client; ret 4): quest 4 start flags, the shared q32::questStartFlags shape: D2Common #10174 reads of quest 4 states 0, 0xF,
// 0xD, 0xE decide whether D2Game+0x354F0 (AL = 4, ECX = client, stack game) fires. Returns 0.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x62A80
uint32_t d2game_62A80_q32(const Call& X, uint32_t game, uint32_t client) { return questStartFlags(X, game, client, 4); }
