// 1.13c D2Game+0x99620 -- the pet/companion spawn-effect (ECX = game, EDX = unit, stack = skill row index, a second stack word unused; ret 8).
// Written from the 1.13c disassembly as a rule over E. idx must lie in the skill table (sgptDataTables word at +0xB98 base, +0xBA0 count, 0x23C-byte rows).
// A stat list is allocated for the unit (0xAABA); unit+0x2C (pData) must be set. 0xE07A0(EAX unit, ECX game) runs, then the unit's target u2 = 0xA598(pData).
// u2 not the unit, non-null and 0xA754(unit, u2, 0) true: a player unit (type 0) hands off to 0xA2DE0(EAX unit, EBX game, u2) and returns its result; any other
// unit reports through the 8-word packet {n, unit, u2, 0, 0, flag << 8, 0, 0} (n = 5 for class 0x49, 5 for 0xD3, else 4; flag 0x65 when 0x74F20(n, unit) is true)
// to 0x75E10(game, &pkt, 1) and returns that. Otherwise the list gets owner state: 0xA68E(u2, 0x36) clear sets 0xAC94(list, [u2]) and 0xACDC(list, [u2+0xC]);
// else 0xAC94(list, 6) and 0xACDC(list, 0). Then 0xAB20(list, 7), the scale (player: class record byte +0x41 << 8 via 0x10A0; other: record 0x1220 word +0x34 << 8,
// default 0x100, with 0xAB20(list, 0xF), 0xAF6A(pData), 0xAB14(pData, 0x14) when the record exists), the skill level (0xA42A(unit, 0x43, 0), at least 0x32) added to the
// 0x1B90 table value, scaled by 0x14D0 and set with 0xACAC; 0xAAF0, 0xA78A(list, 0x1001), 0xA3CA(unit, 0x12, 1); returns 1. `stale` is the frame word at the packet slot
// (R-0x20), which retail reads uninitialised on this path.
#ifndef D2GAME_RULE_99620
#define D2GAME_RULE_99620
#include <cstdint>

namespace g99620 {
template <class E> uint32_t Spawn(E& e, uint32_t game, uint32_t unit, uint32_t idx, uint32_t g, uint32_t stale)
{
    const uint32_t tbl = e.rd32(g);
    if ((int32_t)idx < 0 || (int32_t)idx >= (int32_t)e.rd32(tbl + 0xBA0) || idx * 0x23Cu + e.rd32(tbl + 0xB98) == 0) return 0;
    const uint32_t sl = e.h(0xAABA, 0, 0, 0, 0, 0, unit);
    if (!sl) return 0;
    const uint32_t pd = e.rd32(unit + 0x2C);
    if (!pd) return 0;
    e.h(0xE07A0, unit, game);
    const uint32_t u2 = e.h(0xA598, 0, 0, 0, 0, 0, pd);
    uint32_t v = 0;
    bool owner = false;
    if (u2 != unit && u2) {
        if (e.h(0xA754, 0, 0, 0, 0, 0, unit, u2, 0)) {
            if (e.rd32(unit) == 0) return e.h(0xA2DE0, unit, 0, 0, game, 0, u2);
            e.h(0xDF980, game, unit, 0, 0, 0, 1);
            e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
            const uint32_t v1 = e.h(0x18A0, unit), v2 = e.h(0x1850, v1);
            const uint32_t n = v2 == 0x49 ? 5u : (v2 == 0xD3) + 4u;
            e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);   // 0x75070 (ESI = packet, EDI = unit, stack n) runs natively
            const uint32_t f = e.h(0x74F20, n, unit);
            uint32_t pkt[8] = { n, unit, u2, 0, 0, (f ? 0x65u : 0x64u) << 8, 0, 0 };
            const uint32_t r = e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(pkt), 1);
            e.blk(0x75E10, pkt, 8);
            return r;
        }
        if (!e.h(0xA68E, 0, 0, 0, 0, 0, u2, 0x36)) { e.h(0xAC94, 0, 0, 0, 0, 0, sl, e.rd32(u2)); v = e.rd32(u2 + 0xC); owner = true; }
    }
    if (!owner) e.h(0xAC94, 0, 0, 0, 0, 0, sl, 6);
    e.h(0xACDC, 0, 0, 0, 0, 0, sl, v);
    e.h(0xAB20, 0, 0, 0, 0, 0, sl, 7);
    int32_t scale = 0x100;
    if (e.rd32(unit) == 0) {
        const uint32_t rec = e.h(0x10A0, e.rd32(unit + 4));
        if (rec) scale = (int32_t)(e.rd8(rec + 0x41) << 8);
    } else {
        const uint32_t cls = e.rd32(unit + 4), rec = e.h(0x1220, cls);
        if (rec) {
            scale = e.rd16s(rec + 0x34) * 256;
            if (cls == 0xD3) e.h(0xAB20, 0, 0, 0, 0, 0, sl, 0xF);
            e.h(0xAF6A, 0, 0, 0, 0, 0, pd);
            e.h(0xAB14, 0, 0, 0, 0, 0, pd, 0x14);
        }
    }
    int32_t lv = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, unit, 0x43, 0);
    if (lv <= 0x32) lv = 0x32;
    const uint32_t t = e.h(0x1B90, idx, 0, 0, 0, 0, 0xB2000000u | 0xFBA14, 0x330);
    const uint32_t m = e.h(0x14D0, t + (uint32_t)lv, 100, (uint32_t)scale);
    e.h(0xACAC, 0, 0, 0, 0, 0, stale, m);
    e.h(0xAAF0, 0, 0, 0, 0, 0, stale, unit, 0);
    e.h(0xA78A, 0, 0, 0, 0, 0, sl, 0x1001);
    e.h(0xA3CA, 0, 0, 0, 0, 0, unit, 0x12, 1);
    return 1;
}
}
#endif
