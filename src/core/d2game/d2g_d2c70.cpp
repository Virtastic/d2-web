// 1.13c D2Game+0xD2C70: EAX = owner unit, ESI = item, [esp+4] = game,
// ret 4. When the item is the owner's cursor item (thunk 0xA6A6 on the inventory +0x60), it is taken off the cursor: a player
// owner's client (player data +0x9C via thunk 0xA4C0) is resolved, thunk 0xAA5A(&zero dword, 0x20, 1), 0x8AF10 (EAX = owner,
// stack = client), thunk 0xAA54(inventory, 0), 0xDFC10 (EBX = the 0xE7E0 callback, stack = item) and 0xE20A0 (ECX = game,
// EDX = item); the result is 1, else 0.
#ifndef D2G_D2C70_CPP
#define D2G_D2C70_CPP
#include "d2g_mem.h"
namespace d2g_d2c70 {
using namespace d2g;
struct Env {
    U (*cursorItem)(U inv);                  // thunk 0xA6A6 (stdcall 1)
    P (*playerData)(P unit);                 // thunk 0xA4C0 (stdcall 1)
    void (*a5a)(U* word, I id, I one);       // thunk 0xAA5A (stdcall 3)
    void (*f8af10)(P unit, U client);        // 0x8AF10: EAX = unit, stack word, ret 4
    void (*a54)(U inv, I zero);              // thunk 0xAA54 (stdcall 2)
    void (*fdfc10)(U cb, U item);            // 0xDFC10: EBX = callback (module 0xE7E0), stack word, ret 4
    void (*fe20a0)(U game, U item);          // 0xE20A0: fastcall
    U cbE7E0;                                // the callback's address as the caller sees it
};
// 1.13c D2Game+0xD2C70
inline I RemoveItemIfOnCursor_113c(const Env& e, P unit, U item, U game) {
    const U inv = rd(unit, 0x60);
    if (!item || e.cursorItem(inv) != item) return 0;
    const U client = rd(unit, 0) == 0 ? rd(e.playerData(unit), 0x9C) : 0;
    U word = 0;
    e.a5a(&word, 0x20, 1);
    e.f8af10(unit, client);
    e.a54(inv, 0);
    e.fdfc10(e.cbE7E0, item);
    e.fe20a0(game, item);
    return 1;
}
}
#endif
