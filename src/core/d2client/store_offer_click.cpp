// What a click on an offer in an open store's grid does, as retail's store handlers decide it.
// 1.13c D2Client:
//   +0x9BED0 (store WM_LBUTTONDOWN) runs only in shop modes 0..4 (DAT_6fbcbc34). With the cursor
//            empty, a press inside the store grid (DAT_6fbbb3b4..c0) goes to the grid click +0x96AA0
//            on the vendor's inventory; the item is not the player's, so it reaches the trade action
//            +0x95F80 with ESI = 0, and +0x47D60 builds the Buy transaction (DAT_6fbc9729 = 1, the cost
//            from D2Common #10107, type 2 at a gambler) and opens the confirm box +0x47500:
//            "Buy" (0xd14), the item's name, "Gold:", the cost, Yes / No. Nothing is sent before Yes.
//   +0x9C830 (store WM_RBUTTONDOWN): a targeting cursor (DAT_6fbcb864 6 or 8) is cancelled first.
//            With the cursor empty and the shop in mode 1 or 2, a press inside the store grid over the
//            hovered offer (DAT_6fbcbc38) calls +0x95F80 with ESI = 1, and +0x47D60 sends the buy at
//            once through +0x46EE0 -- unless less than 500 ms (GetTickCount) have passed since the
//            last trade packet +0x46EE0 sent (DAT_6fbd2309). In mode 3 or 4 (Sell / Repair armed), or
//            with an item on the cursor in modes 2..4, the right press ends that mode (+0x8D300).
//   +0x47D60's Buy branch refuses when the player's cursor holds an item (D2Common #11017).
// Not modelled: the identify cursor over a store offer (+0x96AA0's 0x27 branch, which the server
// refuses for a vendor's item) and +0x47D60's vendor-class switch (every store the shell opens is one).
#include <cstdint>
#if __has_include(<emscripten.h>)
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" {
// Result: 0 nothing, 1 open the Buy confirm (+0x47500 state 1), 2 buy at once (+0x46EE0),
// 3 end the armed Sell / Repair mode (+0x8D300).
// button 0 left / 2 right; shopMode = DAT_6fbcbc34; cursorState = DAT_6fbcb864;
// cursorItem = an item on the cursor; hasOffer = an offer under the press inside the store grid;
// nowMs / lastSendMs = GetTickCount now and at the last trade send (DAT_6fbd2309, 0 before any).
// 1.13c D2Client+0x9BED0 / +0x9C830 / +0x95F80 / +0x47D60
EMSCRIPTEN_KEEPALIVE int32_t d2_client_store_offer_click(int32_t button, int32_t shopMode,
                                                         int32_t cursorState, int32_t cursorItem,
                                                         int32_t hasOffer, uint32_t nowMs,
                                                         uint32_t lastSendMs) {
    if (button == 0) {
        if (shopMode < 0 || shopMode > 4) return 0;   // +0x9BED0: another panel owns the press
        if (cursorItem || !hasOffer) return 0;        // a held item is the sell path, not this rule
        if (cursorState == 6) return 0;               // identify cursor: not modelled
        return 1;                                     // +0x47D60 with ESI = 0: the confirm box
    }
    if (button != 2) return 0;
    if (cursorState == 6 || cursorState == 8) return 0;   // +0x9C830 cancels the targeting cursor
    if (cursorItem) return (shopMode >= 2 && shopMode <= 4) ? 3 : 0;
    if (shopMode == 1 || shopMode == 2) {
        if (!hasOffer) return 0;
        if (static_cast<uint32_t>(nowMs - lastSendMs) < 500u) return 0;   // +0x47D60's gate
        return 2;
    }
    if (shopMode == 3 || shopMode == 4) return 3;
    return 0;
}
}
