// QUESTS_DeleteItemEx: remove a quest item from a player, by the item's mode (0 stored, 1 equipped, 4 on cursor).
// Register args: EAX = item, ECX = player, stack = game, inventory (ret 8). Retail-layout (32-bit offsets) rule; every engine callee is an Env member.
#include "d2game_2e790.h"
using namespace d2g32;

struct D2G365E0Env {
    uint32_t (*playerData)(uint32_t player);                          // D2Common #10920 (thunk 0xA4C0), stdcall; client at +0x9C
    uint32_t (*invPage)(uint32_t item);                               // thunk 0xA6C4
    void (*setItemCell)(uint32_t item, uint32_t page);                // thunk 0xA958
    void (*updateClientItem)(uint32_t item, uint32_t client, uint32_t player, uint32_t flags);   // 0x8C810: EAX item, EBX client, EDI player
    void (*removeItem)(uint32_t player, uint32_t game, uint32_t zero);   // 0xF2D90: EAX player
    uint32_t (*itemsXPosition)(uint32_t inventory, uint32_t item);    // thunk 0xA98E
    void (*unequip)(uint32_t game, uint32_t player, uint32_t x, uint32_t one, uint32_t* out);   // 0xF8970: ECX game, EDX player
    void (*removeCursor)(uint32_t player, uint32_t game);             // 0xF2C70: EAX player
    void (*setCursorItem)(uint32_t inventory, uint32_t item);         // thunk 0xAA54
};

// 1.13c D2Game+0x365E0
void QUESTS_DeleteItemEx(uint32_t game, uint32_t player, uint32_t inventory, uint32_t item, const D2G365E0Env& env, uint32_t* slot)
{
    uint32_t client = 0;
    if (player && rd32(player) == 0) client = rd32(env.playerData(player) + 0x9C);
    uint32_t flags = 0;
    flags |= 0x20;                                  // D2Common #10758 SetOrClearFlags(&flags, 0x20, 1): the one import called for real
    const uint32_t mode = item ? rd32(item + 0x10) : 0;
    if (mode == 0) {
        env.setItemCell(item, env.invPage(item));
        env.updateClientItem(item, client, player, flags);
        env.removeItem(player, game, 0);
    } else if (mode == 1) {
        const uint32_t x = env.itemsXPosition(inventory, item);
        env.unequip(game, player, x, 1, slot);      // out pointer = the dead inventory argument slot (a stack address)
    } else if (mode == 4) {
        env.removeCursor(player, game);
        env.setCursorItem(inventory, 0);
    }
}
