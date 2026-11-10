// 1.13c D2Game+0xD36D0: EDI = target stack item T, stack = game, owner unit, source stack item S (ret 0xC). The fatal paths (a negative
// quantity or maximum: 0x114C, 0x114E, 0x1150, 0x1159, 0x1177) leave the process and are not modelled. Thunk 0xA88C(S, T) gates it. The two
// quantities (stat 0x46, thunk 0xA42A) are pooled up to T's maximum (thunk 0xAA06): over the maximum T is capped at it, S keeps the rest and T is
// tail-called through thunk 0xA65E(T, 8, 1); within the maximum S's quantity is added to T (T's stat 0x48 is raised to S's when thunk 0xA970 says
// S has one and S's is lower... see code), the cursor side is refreshed and the game's 0xEE70 pass runs on S. Every stat change is sent with
// 0x8B010 and the quantity record update with 0xD31D0.
#ifndef D2G_D36D0_CPP
#define D2G_D36D0_CPP
#include "d2g_mem.h"
namespace d2g_d36d0 {
using namespace d2g;
struct Env {
    I (*a88c)(P s, P t);                                      // thunk 0xA88C (2)
    I (*a42a)(P item, I stat, I z);                           // thunk 0xA42A (3)
    I (*aa06)(P item);                                        // thunk 0xAA06 (1)
    void (*a472)(P item, I stat, I val, I z);                 // thunk 0xA472 (4)
    P (*a4c0)(P unit);                                        // thunk 0xA4C0 (1)
    void (*f8b010)(P item, U ebx, U client, I one, I stat, I val);   // 0x8B010: EAX, EBX, 4 words
    I (*aa66)(P item);                                        // thunk 0xAA66 (1)
    void (*fd31d0)(P item, P unit, I amount);                 // 0xD31D0: EAX, 2 words
    void (*a65e)(P item, I a, I b);                           // thunk 0xA65E (3), tail call
    I (*a970)(P item);                                        // thunk 0xA970 (1)
    void (*aa54)(U inv, I z);                                 // thunk 0xAA54 (2)
    void (*f8af10)(P unit, U client);                         // 0x8AF10: EAX = unit, 1 word
    void (*fee70)(P game, P item);                            // 0xEE70: EAX = game, ESI = item
};
inline void Merge_113c(const Env& e, P t, P game, P unit, P s) {
    if (!e.a88c(s, t)) return;
    const U inv = rd(unit, 0x60);
    const I cs = e.a42a(s, 0x46, 0);
    const I ct = e.a42a(t, 0x46, 0);
    const I mx = e.aa06(t);
    auto client = [&]() -> U { return rd(unit, 0) == 0 ? rd(e.a4c0(unit), 0x9C) : 0; };
    const I sum = (I)((U)ct + (U)cs);
    if (sum > mx) {
        e.a472(t, 0x46, mx, 0);
        e.f8b010(t, 0, client(), 1, 0x46, mx);
        const I rest = (I)((U)ct - (U)mx + (U)cs);
        e.a472(s, 0x46, rest, 0);
        e.f8b010(s, 0, client(), 1, 0x46, rest);
        if (e.aa66(s) == 0x12 && e.aa66(t) == 0x12) e.fd31d0(t, unit, (I)((U)e.aa06(t) - (U)ct));
        e.a65e(t, 8, 1);
        return;
    }
    if (e.a970(s)) {
        const I x = e.a42a(s, 0x48, 0);
        const I y = e.a42a(t, 0x48, 0);
        if (x < y) {
            e.a472(t, 0x48, x, 0);
            e.f8b010(t, 0, client(), 1, 0x48, x);
        }
    }
    e.a472(t, 0x46, sum, 0);
    e.f8b010(t, 0, client(), 1, 0x46, sum);
    if (e.aa66(s) == 0x12 && e.aa66(t) == 0x12) e.fd31d0(t, unit, cs);
    e.aa54(inv, 0);
    e.f8af10(unit, client());
    e.fee70(game, s);
}
}
#endif
