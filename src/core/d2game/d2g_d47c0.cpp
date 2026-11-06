// 1.13c D2Game+0xD47C0: ESI = owner unit, EDI = item, stack force; ret 4.
// No owner, no item or a non-item (type 4) returns. A player owner in state 7 (thunk 0xA68E) returns. A gem (type 0x14)
// or rune (0x4A, thunk 0xA430) socketed in an item owner applies its gems.txt properties (item record thunk 0xA53E, gem
// record +0xF0 via thunk 0xA87A, apply type thunk 0xA9E2(owner, 0), thunk 0xA9D0 with mode 2 / owner 0, or mode 5 / owner);
// a non-item owner of a gem or rune returns. Any other set item (quality 5, thunk 0xA898) updates sets (thunk 0xA96A).
// Then 0xD4670 (EAX item, ECX owner) runs for every owner (1.10f: players only, flag 1). The main-hand flag is 0 when the
// owner can dual wield (thunk 0xA856), the item is not the player's left-hand weapon (thunk 0xA54A on +0x60) and is a weapon
// (0x2D); unless the item is broken (thunk 0xA8A4 flag 0x100) and force is 0, the stat lists merge (thunk 0xA8C2). An owner
// with a dynamic path (+0x2C) updates its anim rate (thunk 0xA59E).
#ifndef D2G_D47C0_CPP
#define D2G_D47C0_CPP
#include "d2g_mem.h"
namespace d2g_d47c0 {
using namespace d2g;
struct Env {
    I (*state)(P unit, I st);                         // thunk 0xA68E (stdcall 2)
    I (*isType)(P item, I type);                      // thunk 0xA430 (stdcall 2)
    P (*itemRec)(U cls);                              // thunk 0xA53E (stdcall 1)
    U (*gemRec)(U off);                               // thunk 0xA87A (stdcall 1)
    U (*applyType)(P unit, I z);                      // thunk 0xA9E2 (stdcall 2)
    void (*assign)(I mode, U unit, P item, U gem, U apply); // thunk 0xA9D0 (stdcall 5)
    I (*quality)(P item);                             // thunk 0xA898 (stdcall 1)
    void (*sets)(P unit, P item, I a, I b);           // thunk 0xA96A (stdcall 4)
    void (*f4670)(P item, P unit);                    // 0xD4670: EAX item, ECX unit
    U (*leftHand)(U inv);                             // thunk 0xA54A (stdcall 1)
    I (*dual)(P unit);                                // thunk 0xA856 (stdcall 1)
    I (*flag)(P item, U f, I line, U file);           // thunk 0xA8A4 (stdcall 4)
    void (*merge)(P unit, P item, I main);            // thunk 0xA8C2 (stdcall 3)
    void (*anim)(P unit, U file, I line);             // thunk 0xA59E (stdcall 3)
    U file;                                           // the source-file string as the caller sees it
};
// 1.13c D2Game+0xD47C0
inline void UpdateItemStatlist_113c(const Env& e, P unit, P item, I force) {
    if (!unit || !item || rd(item, 0) != 4) return;
    if (rd(unit, 0) == 0 && e.state(unit, 7)) return;
    const bool gem = e.isType(item, 0x14) != 0;
    if (gem || e.isType(item, 0x4A)) {
        if (rd(unit, 0) != 4) return;
        P ir = e.itemRec(rd(item, 4));
        if (!ir) return;
        const U gr = e.gemRec(rd(ir, 0xF0));
        if (!gr) return;
        const U ap = e.applyType(unit, 0);
        if (gem) e.assign(2, 0, item, gr, ap);
        else e.assign(5, W(unit), item, gr, ap);
    } else if (e.quality(item) == 5) {
        e.sets(unit, item, 0, 0);
    }
    e.f4670(item, unit);
    const U left = rd(unit, 0) == 0 ? e.leftHand(rd(unit, 0x60)) : 0;
    I main = 1;
    if (e.dual(unit)) {
        if (W(item) != left && e.isType(item, 0x2D)) main = 0;
        if (!e.flag(item, 0x100, 0x2B9, e.file) || force) e.merge(unit, item, main);
    } else {
        if (!e.flag(item, 0x100, 0x2C2, e.file) || force) e.merge(unit, item, main);
    }
    if (rd(unit, 0x2C)) e.anim(unit, e.file, 0x2D0);
}
}
#endif
