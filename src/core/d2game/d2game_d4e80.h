// D2Game+0xD4E80 seams: move an item from the ground/cursor into a player's inventory grid, each callee reachable through
// the table so the FN replay can hook it.
#pragma once
#include <cstdint>
struct D2GameD4E80Env {
    void (*sync)(void* player);                                           // 0xD29F0 (stack player), ret 4
    uint32_t (*itemCheck)(void* item);                                    // 0xA8D4 D2Common 10792, ret 4
    uint32_t (*invPlace)(uint32_t inv, void* item);                       // 0xAA30 D2Common 10729, ret 8
    void (*itemPos)(void* item, uint32_t* x, uint32_t* y, uint32_t line);  // 0xA85C D2Common 10114 (item, &x, &y, file, line), ret 0x14
    uint32_t (*invPlaceAt)(uint32_t inv, void* item, uint32_t page);      // 0xA9E8 D2Common 10655, ret 0xC
    void (*itemSetPage)(void* item, uint32_t page);                       // 0xA92E D2Common 10012, ret 8
    uint32_t (*invAdd)(uint32_t inv, void* item, uint32_t mode);          // 0xA9B8 D2Common 10405, ret 0xC
    void (*invSetCursor)(uint32_t inv, uint32_t item);                    // 0xAA54 D2Common 10466, ret 8
    void (*picked)(void* item, void* player);                             // 0xD4670 (EAX item, ECX player), ret
    uint32_t (*itemOwned)(void* item, void* player);                      // 0xA640 D2Common 11057, ret 8
    void (*refresh)(void* player, void* item, uint32_t flag);             // 0xD47C0 (ESI player, EDI item, stack flag), ret 4
    void (*itemSetMode)(void* item, uint32_t mode);                       // 0xA4A2 D2Common 11090, ret 8
    void (*itemFlag)(void* item, uint32_t flag, uint32_t on);             // 0xA916 D2Common 10892, ret 0xC
    void (*unitUpdate)(void* player, uint32_t on);                        // 0xA9A6 D2Common 10201, ret 8
    void (*invLink)(uint32_t inv, void* item);                            // 0xA994 D2Common 10050, ret 8
};
// EAX = item id, ECX = player, EDX = game; stack page, direct, out (ret 0xC). A null game or player is retail's assert exit.
uint32_t d2game_d4e80(const D2GameD4E80Env& env, uint32_t itemId, void* player, void* game, uint32_t page, uint32_t direct, uint32_t* out);
