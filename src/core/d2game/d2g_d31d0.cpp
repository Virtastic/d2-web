// 1.13c D2Game+0xD31D0: EAX = the owner guard (ESI), stack = item, addend (ret 8). The fatal paths (item null 0x203, EAX zero
// 0x204) leave the process and are not modelled. A slot from 0xD1AA0 (ESI = guard) other than -1 fetches the quantity record via
// thunk 0xA3D0 (ECX = item, EDX = slot, word -1); count (0xA8E0) + addend is stored (0xA862); the record of a type-0 item's
// player data (thunk 0xA4C0, +0x9C) goes with it to 0x8D570 (ECX = slot, EDX = item[3], AL = item[0], ESI = player data, word = new count).
#ifndef D2G_D31D0_CPP
#define D2G_D31D0_CPP
#include "d2g_mem.h"
namespace d2g_d31d0 {
using namespace d2g;
struct Env {
    I (*f1aa0)(U esi);                         // 0xD1AA0: ESI = guard, no stack
    P (*a3d0)(P item, I slot, I m1);           // thunk 0xA3D0: ECX = item, EDX = slot, word -1
    I (*a8e0)(P rec);                          // thunk 0xA8E0 (1)
    void (*a862)(P rec, I count);              // thunk 0xA862 (2)
    P (*a4c0)(P item);                         // thunk 0xA4C0 (1)
    void (*f8d570)(U al, U slot, U item3, U esi, U count);   // 0x8D570: AL, ECX, EDX, ESI, word
};
inline void SendQuantity_113c(const Env& e, U eax, P item, I addend) {
    const I slot = e.f1aa0(eax);
    if (slot == -1) return;
    P rec = e.a3d0(item, slot, -1);
    if (!rec) return;
    const U nc = (U)e.a8e0(rec) + (U)addend;
    e.a862(rec, (I)nc);
    const U t = rd(item, 0), u = rd(item, 0xC);
    const U client = t == 0 ? rd(e.a4c0(item), 0x9C) : 0;
    e.f8d570(t, (U)slot, u, client, nc);
}
}
#endif
