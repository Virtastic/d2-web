// 1.13c D2Game+0xD3CB0: stack = game, owner unit, source stack item S (ret 0xC); result 1 once S has been merged whole into a target,
// 0 when S has no quantity or no target stack exists. The loop runs while S's quantity (stat 0x46, thunk 0xA42A) is positive: the target
// comes from thunk 0xA976 (when 0xAA18 says so, else 0xA8B0; the previous target is passed back in) after 0xAA48 gates; 0xA88C accepts it.
// Over the target's maximum (0xAA06) the target is capped at it, a 0x22-byte packet (header 0x3E, the target id, 0x46, the maximum) is
// sent with 0x8A3E0, S keeps the rest and the loop goes round again; within it the target takes the sum (and the 0x48 stat when 0xA970
// says so), S is unlinked (0xA478, 0xA44E, 0xA550, 0xA63A), its flag 2 cleared, 0xDFC10, 0xE20A0 and 0xAA54 run and the result is 1.
// The fatal paths (a negative maximum 0x5C8, remainder 0x5CD, sum 0x5D8, or the loop ending after a pass 0x607) leave the process and
// are not modelled.
#ifndef D2G_D3CB0_CPP
#define D2G_D3CB0_CPP
#include "d2g_mem.h"
namespace d2g_d3cb0 {
using namespace d2g;
struct Env {
    I (*a42a)(P item, I stat, I z);                           // thunk 0xA42A (3)
    U (*aa48)(P item);                                        // thunk 0xAA48 (1)
    U (*aa18)(P item);                                        // thunk 0xAA18 (1)
    P (*a976)(U inv, P item, P prev);                         // thunk 0xA976 (3)
    P (*a8b0)(U inv, P item, P prev);                         // thunk 0xA8B0 (3)
    U (*a88c)(P item, P t);                                   // thunk 0xA88C (2)
    I (*aa06)(P item);                                        // thunk 0xAA06 (1)
    void (*a472)(P item, I stat, I val, I z);                 // thunk 0xA472 (4)
    P (*a4c0)(P unit);                                        // thunk 0xA4C0 (1)
    void (*a2b6)(U* w, U* buf, I n);                          // thunk 0xA2B6 (3), stack locals
    void (*a2ce)(U* w, U v, I n);                             // thunk 0xA2CE (3), writes bits
    U (*a2e0)(U* w);                                          // thunk 0xA2E0 (1), returns the byte count
    void (*f8a3e0)(U client, U* pkt, I n);                    // 0x8A3E0: EAX = client, 2 words
    U (*aa66)(P item);                                        // thunk 0xAA66 (1)
    void (*fd31d0)(P item, P unit, I amount);                 // 0xD31D0: EAX, 2 words
    U (*a970)(P item);                                        // thunk 0xA970 (1)
    void (*f8b010)(P item, U ebx, U client, I one, I stat, I val);   // 0x8B010: EAX, EBX, 4 words
    U (*a478)(P item);                                        // thunk 0xA478 (1)
    void (*a44e)(U r, U type, U id);                          // thunk 0xA44E (3)
    void (*a550)(P item);                                     // thunk 0xA550 (1)
    void (*a63a)(P item);                                     // thunk 0xA63A (1)
    void (*ffc10)(P item);                                    // 0xDFC10: EBX = 0x6FC2E7E0, 1 word
    void (*fe20a0)(P game, P item);                           // 0xE20A0: ECX = game, EDX = item
    void (*aa54)(U inv, I z);                                 // thunk 0xAA54 (2)
};
inline U Merge_113c(const Env& e, P game, P unit, P item) {
    const U inv = rd(unit, 0x60);
    P t = nullptr;
    auto client = [&]() -> U { return rd(unit, 0) == 0 ? rd(e.a4c0(unit), 0x9C) : 0; };
    if (e.a42a(item, 0x46, 0) <= 0) return 0;
    for (;;) {
        if (e.aa48(item)) {
            if (e.aa18(item)) {
                t = e.a976(inv, item, t);
                if (!t) t = e.a8b0(inv, item, nullptr);
            } else {
                t = e.a8b0(inv, item, t);
            }
        }
        if (!t) return 0;
        if (e.a88c(item, t)) {
            const I cs = e.a42a(item, 0x46, 0);
            const I ct = e.a42a(t, 0x46, 0);
            I mx = e.aa06(t);
            const I sum = (I)((U)cs + (U)ct);
            if (sum > mx) {
                mx = e.aa06(t);
                e.a472(t, 0x46, mx, 0);
                const U cl = client();
                U w[8] = {}; U buf[9] = {};
                e.a2b6(w, buf, 0x20);
                const U id = t ? rd(t, 0xC) : 0xFFFFFFFFu;
                if (id < 0x100 && t) {
                    e.a2ce(w, 0, 1); e.a2ce(w, id, 8);
                } else {
                    e.a2ce(w, 1, 1);
                    if (id < 0x10000) { e.a2ce(w, 0, 1); e.a2ce(w, id, 0x10); }
                    else { e.a2ce(w, 1, 1); e.a2ce(w, id, 0x20); }
                }
                e.a2ce(w, 1, 1);
                e.a2ce(w, 0x46, 9);
                if ((U)mx < 0x100) { e.a2ce(w, 0, 1); e.a2ce(w, (U)mx, 8); }
                else {
                    e.a2ce(w, 1, 1);
                    if ((U)mx < 0x10000) { e.a2ce(w, 0, 1); e.a2ce(w, (U)mx, 0x10); }
                    else { e.a2ce(w, 1, 1); e.a2ce(w, (U)mx, 0x20); }
                }
                e.a2ce(w, 0, 1);
                e.a2ce(w, 0, 8);
                e.a2e0(w);
                e.f8a3e0(cl, buf, 0x22);
                const I rest = (I)((U)ct - (U)mx + (U)cs);
                e.a472(item, 0x46, rest, 0);
                if (e.aa66(item) == 0x12 && e.aa66(t) == 0x12) e.fd31d0(t, unit, (I)((U)e.aa06(t) - (U)ct));
            } else {
                if (e.a970(item)) {
                    const I qa = e.a42a(item, 0x48, 0);
                    const I qb = e.a42a(t, 0x48, 0);
                    if (qa < qb) {
                        e.a472(t, 0x48, qa, 0);
                        e.f8b010(t, 0, client(), 1, 0x48, qa);
                    }
                }
                e.a472(t, 0x46, sum, 0);
                e.a472(item, 0x46, 0, 0);
                e.f8b010(t, 0, client(), 1, 0x46, sum);
                if (e.aa66(item) == 0x12 && e.aa66(t) == 0x12) e.fd31d0(t, unit, cs);
                const U r = e.a478(item);
                if (r) {
                    e.a44e(r, rd(item, 0), rd(item, 0xC));
                    e.a550(item);
                    e.a63a(item);
                }
                wr(item, 0xC4, rd(item, 0xC4) & ~2u);
                e.ffc10(item);
                e.fe20a0(game, item);
                e.aa54(inv, 0);
                return 1;
            }
        }
        if (e.a42a(item, 0x46, 0) <= 0) return 0;   // retail leaves the process here after a pass (0x607); not modelled
    }
}
}
#endif
