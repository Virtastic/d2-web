// What a left click with an EMPTY cursor on the player's own inventory does, as retail's inventory
// handlers decide it. 1.13c D2Client:
//   +0x98DC0 dispatches a press inside the inventory panel: the grid to +0x96AA0, a body slot through
//            the per-slot table at +0xE0EC4 (bodyLoc 1-3 and 6-10 -> +0x95FE0, 4 right hand -> +0x96690,
//            5 left hand -> +0x96280).
//   Every handler first tests the cursor state DAT_6fbcb864: 6 is item-on-item targeting (an identify
//   scroll or tome armed), which sends 0x27 for the item under the click.
//   With the cursor empty and an item there (D2Common #10946 result 3 on a slot), the shop mode
//   DAT_6fbcbc34 decides: a pick-up mode (+0x8BE10 for slots, +0x8BE60 for the grid) lifts the item
//   (0x1C from a slot, 0x19 from the grid); any other mode below 0x13 hands the item to the trade action
//   +0x95F80 -> +0x47D60, which sells it in mode 3 and repairs it in mode 4 (0x33 / 0x35 via +0x46EE0).
//   An empty slot or cell does nothing.
//   In the grid's pick-up branch (+0x96AA0) Ctrl held (GetAsyncKeyState 0x11) never lifts: with a vendor's
//   trade open (DAT_6fbc9721 set, its unit found in the hash) the item is sold at once through the
//   sell packet (the refusal sound when #10705 says it cannot be sold); with no vendor it does nothing.
// Not modelled: the left-hand slot showing a two-handed right-hand weapon (#10946 result 4).
#include <cstdint>
#if __has_include(<emscripten.h>)
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

namespace {
// 1.13c D2Client+0x8BE10: the body-slot pick-up modes.
bool slotPickupMode(int32_t m) {
    return m == 0 || m == 1 || m == 2 || m == 0xb || m == 0xc || m == 0xe || m == 0xf || m == 0x13;
}
// 1.13c D2Client+0x8BE60: the grid pick-up modes (the slot set without 0xf).
bool gridPickupMode(int32_t m) {
    return m == 0 || m == 1 || m == 2 || m == 0xb || m == 0xc || m == 0xe || m == 0x13;
}
}  // namespace

extern "C" {
// Result: 0 nothing, 1 pick the item up, 2 the trade action (sell in mode 3, repair in mode 4),
// 3 the targeting cursor's packet (0x27, identify), 4 the Ctrl+click direct sale (grid only).
// cursorState = DAT_6fbcb864, shopMode = DAT_6fbcbc34, bodyLoc 0 = the grid, 1..10 a body slot.
// 1.13c D2Client+0x95FE0 / +0x96280 / +0x96690 / +0x96AA0
EMSCRIPTEN_KEEPALIVE int32_t d2_client_inv_click_route(int32_t cursorState, int32_t shopMode,
                                                       int32_t bodyLoc, int32_t hasItem,
                                                       int32_t ctrl, int32_t npcOpen) {
    if (!hasItem) return 0;
    if (cursorState == 6) return 3;
    const bool pick = bodyLoc == 0 ? gridPickupMode(shopMode) : slotPickupMode(shopMode);
    if (pick) return (bodyLoc == 0 && ctrl) ? (npcOpen ? 4 : 0) : 1;
    return shopMode < 0x13 ? 2 : 0;
}
}
