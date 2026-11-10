// 1.13c D2Game+0xD3A10: stack = game, owner unit, source stack item S (ret 0xC); result 1 when a target stack was found, else 0. The fatal
// paths (game null 0x623, a negative quantity 0x62F / 0x631 / 0x633, a negative remainder 0x63B, a negative sum 0x646) leave the process
// and are not modelled. Thunk 0xAA72(inventory, S, 0) finds the target T; the two quantities (stat 0x46, thunk 0xA42A) are pooled up to T's
// maximum (thunk 0xAA06): over it T is capped at the maximum and S keeps the rest (the 0xD31D0 record gets the amount T gained), within it
// T takes the sum (the record gets S's old quantity), S is unlinked (0xA478, 0xA44E, 0xA550, 0xA63A), its flag 2 cleared and 0xEE70 runs.
#ifndef D2G_D3A10_CPP
#define D2G_D3A10_CPP
#include "d2g_mem.h"
namespace d2g_d3a10 {
using namespace d2g;
struct Env {
    P (*aa72)(U inv, P s, I z);                               // thunk 0xAA72 (3)
    I (*a42a)(P item, I stat, I z);                           // thunk 0xA42A (3)
    I (*aa06)(P item);                                        // thunk 0xAA06 (1)
    void (*a472)(P item, I stat, I val, I z);                 // thunk 0xA472 (4)
    P (*a4c0)(P unit);                                        // thunk 0xA4C0 (1)
    void (*f8b010)(P item, U ebx, U client, I one, I stat, I val);   // 0x8B010: EAX, EBX, 4 words
    void (*fd31d0)(P item, P unit, I amount);                 // 0xD31D0: EAX, 2 words
    U (*a478)(P item);                                        // thunk 0xA478 (1)
    void (*a44e)(U r, U type, U id);                          // thunk 0xA44E (3)
    void (*a550)(P item);                                     // thunk 0xA550 (1)
    void (*a63a)(P item);                                     // thunk 0xA63A (1)
    void (*fee70)(P game, P item);                            // 0xEE70: EAX = game, ESI = item
    void (*aa54)(U inv, I z);                                 // thunk 0xAA54 (2)
};
inline U Merge_113c(const Env& e, P game, P unit, P s) {
    const U inv = rd(unit, 0x60);
    P t = e.aa72(inv, s, 0);
    if (!t) return 0;
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
        e.fd31d0(t, unit, (I)((U)mx - (U)ct));
        return 1;
    }
    e.a472(t, 0x46, sum, 0);
    e.f8b010(t, 0, client(), 1, 0x46, sum);
    e.fd31d0(t, unit, cs);
    const U r = e.a478(s);
    if (r) {
        const U id = s ? rd(s, 0xC) : 0xFFFFFFFFu, ty = s ? rd(s, 0) : 6;
        e.a44e(r, ty, id);
        e.a550(s);
        e.a63a(s);
    }
    if (s) wr(s, 0xC4, rd(s, 0xC4) & ~2u);
    e.fee70(game, s);
    e.aa54(inv, 0);
    return 1;
}
}
#endif
