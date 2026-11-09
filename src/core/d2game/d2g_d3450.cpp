// 1.13c D2Game+0xD3450 (1.10f sub_6FC4B520): ESI = item, EDI = owner unit, stack quantity; ret 4.
// Adds the quantity to stat 0x46 (thunk 0xA4D2), takes the client from the player data (thunk 0xA4C0, +0x9C) when the
// owner is a player (type 0) and 0 otherwise, reads the new quantity (thunk 0xA42A), sends the stat update 0x8B010
// (EAX item, EBX 0; client, 1, 0x46, value) and runs the book/scroll update 0xD31D0 (EAX item; owner, quantity).
#ifndef D2G_D3450_CPP
#define D2G_D3450_CPP
#include "d2g_mem.h"
namespace d2g_d3450 {
using namespace d2g;
struct Env {
    void (*addStat)(P item, I stat, I v, I layer);      // thunk 0xA4D2 (stdcall 4)
    P (*playerData)(P unit);                            // thunk 0xA4C0 (stdcall 1)
    U (*getStat)(P item, I stat, I layer);              // thunk 0xA42A (stdcall 3)
    void (*send3E)(P item, U ebx, U client, I a, I stat, U v); // 0x8B010: EAX item, EBX; 4 stack words, ret 0x10
    void (*bookQty)(P item, P unit, I qty);             // 0xD31D0: EAX item; 2 stack words, ret 8
};
// 1.13c D2Game+0xD3450
inline void ItemAddQuantity_113c(const Env& e, P item, P unit, I qty) {
    e.addStat(item, 0x46, qty, 0);
    U client = 0;
    if (unit && rd(unit, 0) == 0) client = rd(e.playerData(unit), 0x9C);
    const U v = e.getStat(item, 0x46, 0);
    e.send3E(item, 0, client, 1, 0x46, v);
    e.bookQty(item, unit, qty);
}
}
#endif
