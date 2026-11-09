// D2Game+0xD60D0 seams: move the item a player holds onto a body location, each callee reachable through the table so the
// FN replay can hook it.
#pragma once
#include <cstdint>
struct D2GameD60D0Env {
    void (*sync)(void* player);                                                  // 0xD29F0 (stack player), ret 4
    uint32_t (*canEquip)(void* player, uint32_t loc, void* item, uint32_t force);  // 0xAA78 D2Common 10946, ret 0x10
    uint32_t (*canUse)(void* item, void* player);                                // 0xA934 D2Common 10244 (item, player, 0, 0, 0, 0), ret 0x18
    uint32_t (*invOffhand)(uint32_t inv);                                        // 0xA54A D2Common 10061, ret 4
    void (*unequip)(void* player, uint32_t item);                                // 0xA9BE D2Common 11055, ret 8
    uint32_t (*invEquip)(uint32_t inv, void* item, uint32_t loc);                // 0xA9A0 D2Common 11041, ret 0xC
    uint32_t (*invAdd)(uint32_t inv, void* item, uint32_t mode);                 // 0xA9B8 D2Common 10405, ret 0xC
    void (*itemSetLoc)(void* item, uint32_t loc);                                // 0xA86E D2Common 10058, ret 8
    void (*invUnlink)(uint32_t inv, void* item);                                 // 0xA910 D2Common 10138, ret 8
    void (*refresh)(void* player, void* item, uint32_t flag);                    // 0xD47C0 (ESI player, EDI item, stack flag), ret 4
    void (*invSetCursor)(uint32_t inv, uint32_t item);                           // 0xAA54 D2Common 10466, ret 8
    void (*itemSetMode)(void* item, uint32_t mode);                              // 0xA4A2 D2Common 11090, ret 8
    void (*itemSetPage)(void* item, uint32_t page);                              // 0xA92E D2Common 10012, ret 8
    void (*itemFlag)(void* item, uint32_t flag, uint32_t on);                    // 0xA916 D2Common 10892, ret 0xC
    void (*itemFlagSet)(void* item, uint32_t flag, uint32_t on);                 // 0xA65E D2Common 10977, ret 0xC
    uint32_t (*itemFlagTest)(void* item, uint32_t flag, uint32_t line);          // 0xA8A4 D2Common 10202 (item, flag, line, file), ret 0x10
    void (*invLink)(uint32_t inv, void* item);                                   // 0xA994 D2Common 10050, ret 8
    void (*unitUpdate)(void* player, uint32_t on);                               // 0xA9A6 D2Common 10201, ret 8
    void (*stats)(void* player);                                                 // 0xD28D0 (EAX player), ret
    void (*send)(void* game, void* player, uint32_t a);                          // 0xD51A0 (game, player, 0), ret 0xC
};
// EAX = item id, ECX = player; stack game, loc (low byte the body location), force, out (may be null); ret 0x10.
// A null game or player is retail's assert exit.
uint32_t d2game_d60d0(const D2GameD60D0Env& env, uint32_t itemId, void* player, void* game, uint32_t loc, uint32_t force, uint32_t* out);
