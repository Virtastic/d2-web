// QUESTS_DeleteItem: find the player's quest item by code, then delete it. Register args: EAX = item code, ECX = game, ESI = player.
// Retail-layout (32-bit offsets) rule; the two callees are Env members.
#include "d2game_2e790.h"
using namespace d2g32;

// 1.13c D2Game+0x366A0
void QUESTS_DeleteItem(uint32_t code, uint32_t game, uint32_t player,
                       uint32_t (*findItem)(uint32_t player, uint32_t code),                  // 0xE660: EAX player, stack code
                       void (*deleteItemEx)(uint32_t item, uint32_t player, uint32_t game, uint32_t inventory))   // 0x365E0
{
    const uint32_t item = findItem(player, code);
    if (item) deleteItemEx(item, player, game, rd32(player + 0x60));
}
