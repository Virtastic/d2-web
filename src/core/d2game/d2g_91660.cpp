// 1.13c D2Game+0x91660: EAX = skill record, stack (game, unit, skill, level), ret 0x10.
// A level of 1 or less does nothing. The skill row (sgptDataTables +0xB98, 0x23C bytes, count +0xBA0; 0 out of range)
// gives the per-level bonus at +0x148; bonus = (level-1)*row value is applied as a percentage of thunk 0xA3A0(unit):
// the base grows by base*bonus/100 (the retail split: base > 0x100000 divides base by 100 first, bonus > 0x10000 divides
// bonus first; the 64-bit division arm behind a 0x640 test is unreachable but kept) and is written to stats 7 and 6
// (thunk 0xA472 layer 0). A statlist from thunk 0xA6A0(game+0x1C, 0, 0, unit type or 6, unit guid or -1) is attached
// (0xA60A(unit, list, 1)) and filled from the record's six stat words at +0x54 (value thunk 0xAC9A(unit, calc +0x6C,
// skill, level)) and five at +0x98 (calc +0xA8), each a valid stat id (< +0xBD4) set by 0xA6E2(list, stat, value, 0).
// A record state word +0xE4 in 1..0x2A on a monster unit with data (+0x14) runs 0x23260 (EAX = unit, ESI = game) and
// takes the first free byte of the data's nine at +0x1C; the state's handler (table 0x10E550) is then called
// (ECX = unit, EDX = state, stack 1).
#ifndef D2G_91660_CPP
#define D2G_91660_CPP
#include "d2g_mem.h"
namespace d2g_91660 {
using namespace d2g;
struct Env {
    P dt;                                  // *sgptDataTables
    I (*base)(P unit);                     // thunk 0xA3A0 (stdcall 1)
    void (*setStat)(P unit, I stat, I v, I layer);   // thunk 0xA472 (stdcall 4)
    P (*newList)(U pool, I a, I b, I type, I guid);  // thunk 0xA6A0 (stdcall 5)
    void (*attach)(P unit, P list, I one); // thunk 0xA60A (stdcall 3)
    I (*calc)(P unit, U calc, I skill, I level);     // thunk 0xAC9A (stdcall 4)
    void (*listSet)(P list, I stat, I v, I layer);   // thunk 0xA6E2 (stdcall 4)
    void (*f23260)(P unit, P game);        // 0x23260: EAX = unit, ESI = game
    U (*handler)(I state);                 // dword table 0x10E550[state]
    void (*callHandler)(U fn, P unit, I state, I one);   // that handler: ECX = unit, EDX = state, stack 1
};
// 1.13c D2Game+0x91660
inline void ApplyLevelBonus_113c(const Env& e, P rec, P game, P unit, I skill, I level) {
    if (level <= 1) return;
    I v = 0;
    if (skill >= 0 && skill < rds(e.dt, 0xBA0)) {
        const U row = rd(e.dt, 0xB98) + (U)skill * 0x23C;
        if (row) v = rds((P)(uintptr_t)row, 0x148);
    }
    const I bonus = (I)((U)(level - 1) * (U)v);
    I b = e.base(unit), add;
    if (b > 0x100000) {
        if ((I)((U)b & 0xFFFFFFF0u) < 0x640) add = (I)((int64_t)b * bonus / 100);
        else add = (I)((U)(b / 100) * (U)bonus);
    } else if (bonus > 0x10000) {
        if ((I)((U)bonus & 0xFFFFFFF0u) < 0x640) add = (I)((int64_t)b * bonus / 100);
        else add = (I)((U)(bonus / 100) * (U)b);
    } else {
        add = (I)((U)b * (U)bonus) / 100;
    }
    b = (I)((U)b + (U)add);
    e.setStat(unit, 7, b, 0);
    e.setStat(unit, 6, b, 0);
    P list = e.newList(rd(game, 0x1C), 0, 0, unit ? rds(unit, 0) : 6, unit ? rds(unit, 0xC) : -1);
    if (!list) return;
    e.attach(unit, list, 1);
    for (U i = 0; i < 6; ++i) {
        const int16_t st = (int16_t)rd16(rec, 0x54 + 2 * i);
        if (st < 0 || st >= rds(e.dt, 0xBD4)) continue;
        e.listSet(list, st, e.calc(unit, rd(rec, 0x6C), skill, level), 0);
    }
    for (U i = 0; i < 5; ++i) {
        const int16_t st = (int16_t)rd16(rec, 0x98 + 2 * i);
        if (st < 0 || st >= rds(e.dt, 0xBD4)) continue;
        e.listSet(list, st, e.calc(unit, rd(rec, 0xA8), skill, level), 0);
    }
    const int16_t w = (int16_t)rd16(rec, 0xE4);
    if (w <= 0 || w >= 0x2B || !unit || rd(unit, 0) != 1 || !rd(unit, 0x14)) return;
    P slots = (P)(uintptr_t)(rd(unit, 0x14) + 0x1C);
    if (!slots) return;
    e.f23260(unit, game);
    U k = 0;
    while (k < 9 && rd8(slots, k)) ++k;
    if (k >= 9) return;
    wr8(slots, k, (uint8_t)w);
    const U fn = e.handler(w);
    if (fn) e.callHandler(fn, unit, w, 1);
}
}
#endif
