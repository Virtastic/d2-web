// ECX = game, EDX = player; ret 4. When state 0xD of quest record 0x28 is set and the player is a live player unit (type 0, +0x80 set) whose
// party lookup is not -1, the party walker runs the quest callback over the party (EAX = party, ESI = game, EBX = 0). Returns 0.
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x5AB00
uint32_t d2game_5AB00_q32(const Call& X, uint32_t game, uint32_t player, uint32_t callback) {
    if (!qGet(X, questRec(X, game, player), 0x28, 0xd)) return 0;
    if (!player || !rd32(player, 0x80) || rd32(player, 0) != 0) return 0;
    const uint32_t party = X(0x9BEB0, player);
    if ((party & 0xffffu) == 0xffffu) return 0;
    X(0x9C160, party, 0, 0, 0, game, callback);
    return 0;
}
