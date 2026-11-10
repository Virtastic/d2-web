// 1.13c D2Game+0xD39B0: EAX = ESI (pass-through), ECX = EDI = player unit, stack word 0 = arg (ret 4). ebx = player data
// record [+0x9C] (thunk 0xA4C0) when unit[0] == 0, else 0. A slot is filled by thunk 0xAA5A(&slot, 0x20, 1); 0x8C810 (EAX = esi,
// EBX, EDI = unit, word = slot) then 0xD2D90 (EAX = unit, ESI = esi, words arg, 0) finish. Fatal paths live in the callees.
#ifndef D2G_D39B0_CPP
#define D2G_D39B0_CPP
#include "d2g_mem.h"
namespace d2g_d39b0 {
using namespace d2g;
struct Env {
    P (*a4c0)(P unit);                          // thunk 0xA4C0 (1)
    void (*aa5a)(U* slot, I a, I b);            // thunk 0xAA5A (3), fills the slot
    void (*f8c810)(U esi, U ebx, U slot);       // 0x8C810: EAX = esi, EBX, EDI = unit (implicit), word
    void (*fd2d90)(U unit, U esi, U arg, I z);  // 0xD2D90: EAX = unit, ESI = esi, 2 words
};
inline void Remove_113c(const Env& e, U eax, P unit, U arg) {
    const U client = (unit && rd(unit, 0) == 0) ? rd(e.a4c0(unit), 0x9C) : 0;
    U slot = 0;
    e.aa5a(&slot, 0x20, 1);
    e.f8c810(eax, client, slot);
    e.fd2d90(W(unit), eax, arg, 0);
}
}
#endif
