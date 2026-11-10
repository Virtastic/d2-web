// 1.13c D2Game+0xD34B0: EAX = id of the source stack item, stack = game, owner unit, id of the target stack, out-flag pointer
// (ret 0x10). The fatal paths (game or unit null 0x144F/0x1450, a null cursor-side inventory 0x1482, mismatched class words 0x149D)
// leave the process and are not modelled. Both items come from the game's 128-bucket unit hash (game +0x1720, key +0xC, chain +0xE4).
// The source is taken off the cursor / out of its container when the owner's cursor item is not it, the target quantity record is bumped
// (thunk 0xA4D2) and the update goes out through 0xAB010 and 0xD31D0. Result 1 = merged, 0 = not; *out = 1 on a lookup/class refusal.
#ifndef D2G_D34B0_CPP
#define D2G_D34B0_CPP
#include "d2g_mem.h"
namespace d2g_d34b0 {
using namespace d2g;
struct Env {
    void (*f29f0)(P unit);                                   // 0xD29F0 (1)
    I (*a430)(P item, I what);                               // thunk 0xA430 (2)
    I (*aa66)(P item);                                       // thunk 0xAA66 (1)
    I (*a90a)(P item, I z);                                  // thunk 0xA90A (2), ax compared
    I (*a42a)(P item, I stat, I z);                          // thunk 0xA42A (3)
    I (*aa06)(P item);                                       // thunk 0xAA06 (1)
    I (*fd2c70)(P unit, P item, P game);                     // 0xD2C70: EAX = unit, ESI = item, word game
    P (*a478)(P item);                                       // thunk 0xA478 (1)
    void (*a44e)(P r, U type, U id);                         // thunk 0xA44E (3)
    void (*a550)(P item);                                    // thunk 0xA550 (1)
    void (*a63a)(P item);                                    // thunk 0xA63A (1)
    void (*fee70)(P game, P item);                           // 0xEE70: EAX = game, ESI = item
    void (*aa54)(U inv, I z);                                // thunk 0xAA54 (2)
    void (*a4d2)(P item, I stat, I one, I z);                // thunk 0xA4D2 (4)
    P (*a4c0)(P unit);                                       // thunk 0xA4C0 (1)
    void (*f8b010)(P item, U ebx, U client, I one, I stat, I val);   // 0x8B010: EAX, EBX, 4 words
    void (*fd31d0)(P item, P unit, I one);                   // 0xD31D0: EAX, 2 words
};
inline P lookup(P game, U id) {
    P it = pf(game, 0x1720 + 4 * (id & 0x7F));
    while (it && rd(it, 0xC) != id) it = pf(it, 0xE4);
    return it;
}
inline I Merge_113c(const Env& e, U idA, P game, P unit, U idB, U* out) {
    e.f29f0(unit);
    *out = 0;
    P a = lookup(game, idA);
    if (!a) { *out = 1; return 0; }
    if (rd(a, 0) != 4) return 0;
    const U cls = rd(a, 0x10);
    if (cls != 3 && cls != 4) { *out = 1; return 0; }
    if (!e.a430(a, 0x16)) { *out = 1; return 0; }
    P b = lookup(game, idB);
    if (!b) { *out = 1; return 0; }
    if (rd(b, 0) != 4) return 0;
    if (rd(b, 0x10) != 0) { *out = 1; return 0; }
    if (e.aa66(b) != 0x12) { *out = 1; return 0; }
    const U inv = rd(unit, 0x60);
    const uint16_t x = (uint16_t)e.a90a(b, 0), y = (uint16_t)e.a90a(a, 0);
    (void)x; (void)y;
    if (e.a42a(b, 0x46, 0) >= e.aa06(b)) return 0;
    if (!e.fd2c70(unit, a, game)) {
        P r = e.a478(a);
        if (r) { e.a44e(r, rd(a, 0), rd(a, 0xC)); e.a550(a); e.a63a(a); }
        wr(a, 0xC4, rd(a, 0xC4) & ~2u);
        e.fee70(game, a);
        e.aa54(inv, 0);
    }
    e.a4d2(b, 0x46, 1, 0);
    const U client = rd(unit, 0) == 0 ? rd(e.a4c0(unit), 0x9C) : 0;
    const I v = e.a42a(b, 0x46, 0);
    e.f8b010(b, 0, client, 1, 0x46, v);
    e.fd31d0(b, unit, 1);
    return 1;
}
}
#endif
