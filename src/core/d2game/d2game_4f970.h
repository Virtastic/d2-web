// 1.13c D2Game+0x4F970 (SetSummonPassiveStats: a summon's passive stats, aura events, life bonus, granted skills, umod, overlay and gear
// from its summoning skill). stdcall (game, unit, mon, skill, lvl, ilvl), ret 0x18. Written from the 1.13c disassembly as a rule over E (HookEnv).
// dt = sgptDataTables; ev = the event-callback table 0x1077A8 (50 slots), um = the umod-callback table 0x10E550 (0x2B slots); bm = Fog gdwBitMasks.
// skill 0 returns 0 (a nonzero skill out of range reads a null row: not reached). row = dt+0xB98 + 0x23C * skill. monId = mon ? mon+0xC : -1.
// 1. Five stats row+0x98 (shorts) / calcs row+0xA4: a stat s in [0, dt+0xBD4) with an ItemStatCost row (dt+0xBCC + 0x144 s) gets
//    D2Common SetStat 0xA4D2(mon, s, 0xAC9A(unit, calc, skill, lvl), 0); a row +0x48 > 0 and 0x370E0(EAX mon, EDX 2) == 0 then call
//    0x9E290(EAX row+0x4C, ECX monId, EDX 2, ESI mon; game, row+0x48, s << 16, 0), and again with +0x4E / +0x4A when +0x4A > 0.
// 2. Six stats row+0x54 / calcs row+0x68: v := 0xAC9A(...) nonzero; the first one makes the stat list 0xA6A0(game+0x1C, 0, 0, mon ? mon+0 : 6,
//    monId) (null returns 0) and 0xA60A(mon, list, 1); then 0xA6E2(list, s, v, 0). An ISC row +0x48 > 0 not already in mon's event list
//    (mon+0x90, next +0x1C: +4 == 2, +8 == monId, +0xC == s) registers 0x37190(ESI mon; game, +0x48, s << 16, 0, ev[+0x4C], 2, monId)
//    when 0 <= +0x4C < 0x32 and ev[+0x4C] set, then the +0x4E / +0x4A 0x9E290 call as in 1.
// 3. row+0x80 in (0, dt+0xC4]: 0xA3CA(mon, s80, 1), and with a list 0xA5B6(list, s80).
// 4. Life: c := 0xAC9A(unit, row+0x138, ...), hp := 0xA3A0(mon); hp + hp * c / 100 (split to avoid overflow past 0x100000 / 0x10000)
//    goes to SetStat 0xA472(mon, 7, ., 0) and (mon, 6, ., 0).
// 5. Five granted skills row+0xC4 / calcs row+0xD0: g in (0, dt+0xBA0), n := 0xAC9A(...) > 0, a skill row: 0xA9FA(mon, g, n, 0, file, 0x112A),
//    0xAEB6(mon), a player mon (type 0) 0x96FA0(mon+0x80, mon); a row+4 byte that hits bm[5] gives 0xA2100(ECX mon, EDX 0; g, -1).
// 6. row+0x84 >= 0: 0x37110(EBX game, EDI mon; 1, s80) and up to three events row+0x84 (stop at a negative) with ev[row+0x8A] set:
//    0x37190(ESI mon; game, ev, skill, lvl, ev[k], 1, s80).
// 7. Umod row+0xE4 in (0, 0x2B) on a monster with monster data md: the player counter game+0xF0[md+0x58] +0x2C8 counts once (md+0x16 bit 8
//    clear), md+0x16 |= 8; the first free byte of md+0x1C[0..9) := umod and um[umod] set is called (ECX mon, EDX umod; 1).
// 8. row+0xE6 in (0, dt+0xBC0): 0xA83E(mon, ov, 0). 9. Gear 0x92ED0(EAX lvl, ECX mon; game, unit, i, 0), i = ilvl or
//    min(max(1, 3 * lvl), 0xA42A(unit, 0xC, 0)). Result 1.
#pragma once
#include <cstdint>

namespace g4f970 {
template <class E> uint32_t SetSummonPassiveStats(E& e, uint32_t dt, uint32_t ev, uint32_t um, uint32_t bm,
                                                  uint32_t game, uint32_t unit, uint32_t mon, uint32_t skill, uint32_t lvl, uint32_t ilvl)
{
    // 1.13c D2Game+0x4F970
    const int32_t sk = (int32_t)skill;
    if (!sk) return 0;
    const uint32_t row = (sk >= 0 && sk < (int32_t)e.rd32(dt + 0xBA0)) ? e.rd32(dt + 0xB98) + skill * 0x23C : 0;
    auto monId = [&]() { return mon ? e.rd32(mon + 0xC) : 0xFFFFFFFFu; };
    auto isc = [&](int32_t s) -> uint32_t {
        if (s < 0 || s >= (int32_t)e.rd32(dt + 0xBD4)) return 0;
        return e.rd32(dt + 0xBCC) + (uint32_t)s * 0x144;
    };
    for (uint32_t i = 0; i < 5; i++) {
        const int32_t s = e.rd16s(row + 0x98 + 2 * i);
        const uint32_t ir = isc(s);
        if (!ir) continue;
        const uint32_t v = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(row + 0xA4 + 4 * i), skill, lvl);
        e.h(0xA4D2, 0, 0, 0, 0, 0, mon, (uint32_t)s, v, 0);
        if (e.rd16s(ir + 0x48) <= 0) continue;
        if (e.h(0x370E0, mon, 0, 2)) continue;
        e.h(0x9E290, (uint32_t)e.rd16s(ir + 0x4C), monId(), 2, 0, mon, game, (uint32_t)e.rd16s(ir + 0x48), (uint32_t)s << 16, 0);
        const int32_t a = e.rd16s(ir + 0x4A);
        if (a > 0) e.h(0x9E290, (uint32_t)e.rd16s(ir + 0x4E), monId(), 2, 0, mon, game, (uint32_t)a, (uint32_t)s << 16, 0);
    }
    uint32_t list = 0;
    for (uint32_t i = 0; i < 6; i++) {
        const int32_t s = e.rd16s(row + 0x54 + 2 * i);
        const uint32_t ir = isc(s);
        if (!ir) continue;
        const uint32_t v = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(row + 0x68 + 4 * i), skill, lvl);
        if (!v) continue;
        if (!list) {
            list = e.h(0xA6A0, 0, 0, 0, 0, 0, e.rd32(game + 0x1C), 0, 0, mon ? e.rd32(mon) : 6, monId());
            if (!list) return 0;
            e.h(0xA60A, 0, 0, 0, 0, 0, mon, list, 1);
        }
        e.h(0xA6E2, 0, 0, 0, 0, 0, list, (uint32_t)s, v, 0);
        const int32_t si = e.rd16s(ir + 0x48);
        if (si <= 0) continue;
        uint32_t id = 0xFFFFFFFFu;
        if (mon) {
            id = e.rd32(mon + 0xC);
            bool found = false;
            for (uint32_t n = e.rd32(mon + 0x90); n; n = e.rd32(n + 0x1C))
                if (e.rd32(n + 4) == 2 && e.rd32(n + 8) == id && e.rd32(n + 0xC) == (uint32_t)s) { found = true; break; }
            if (found) continue;
        }
        const int32_t k = e.rd16s(ir + 0x4C);
        if (k >= 0 && k < 0x32 && e.rd32(ev + 4 * k)) {
            uint32_t w[7] = { game, (uint32_t)si, (uint32_t)s << 16, 0, e.rd32(ev + 4 * k), 2, id };
            e.h(0x37190, 0, 0, 0, 0, mon, w[0], w[1], w[2], w[3], w[4], w[5], w[6]);   // s4..s6 are not in the hook record
        }
        const int32_t a = e.rd16s(ir + 0x4A);
        if (a > 0) e.h(0x9E290, (uint32_t)e.rd16s(ir + 0x4E), monId(), 2, 0, mon, game, (uint32_t)a, (uint32_t)s << 16, 0);
    }
    const int32_t s80 = e.rd16s(row + 0x80);
    if (s80 > 0 && s80 <= (int32_t)e.rd32(dt + 0xC4)) {
        e.h(0xA3CA, 0, 0, 0, 0, 0, mon, (uint32_t)s80, 1);
        if (list) e.h(0xA5B6, 0, 0, 0, 0, 0, list, (uint32_t)e.rd16s(row + 0x80));
    }
    const int32_t c = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(row + 0x138), skill, lvl);
    const int32_t hp = (int32_t)e.h(0xA3A0, 0, 0, 0, 0, 0, mon);
    int32_t add;
    if (hp > 0x100000) add = (int32_t)(hp & ~0xF) < 0x640 ? (int32_t)((int64_t)hp * c / 100) : (int32_t)((uint32_t)(hp / 100) * (uint32_t)c);
    else if (c > 0x10000) add = (int32_t)(c & ~0xF) < 0x640 ? (int32_t)((int64_t)hp * c / 100) : (int32_t)((uint32_t)(c / 100) * (uint32_t)hp);
    else add = (int32_t)((uint32_t)hp * (uint32_t)c) / 100;
    const uint32_t life = (uint32_t)hp + (uint32_t)add;
    e.h(0xA472, 0, 0, 0, 0, 0, mon, 7, life, 0);
    e.h(0xA472, 0, 0, 0, 0, 0, mon, 6, life, 0);
    for (uint32_t i = 0; i < 5; i++) {
        const int32_t g = e.rd16s(row + 0xC4 + 2 * i);
        if (g <= 0 || g >= (int32_t)e.rd32(dt + 0xBA0)) continue;
        const int32_t n = (int32_t)e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(row + 0xD0 + 4 * i), skill, lvl);
        if (n <= 0) continue;
        const uint32_t gr = e.rd32(dt + 0xB98) + (uint32_t)g * 0x23C;
        if (!gr) continue;
        e.h(0xA9FA, 0, 0, 0, 0, 0, mon, (uint32_t)g, (uint32_t)n, 0, 0xB20FB980u, 0x112A);   // the source-file string, as the capture tags it
        e.h(0xAEB6, 0, 0, 0, 0, 0, mon);
        if (mon && e.rd32(mon) == 0) e.h(0x96FA0, 0, 0, 0, 0, 0, e.rd32(mon + 0x80), mon);
        if (e.rd8(bm + 0x14) & e.rd8(gr + 4)) e.h(0xA2100, 0, mon, 0, 0, 0, (uint32_t)g, 0xFFFFFFFFu);
    }
    if (e.rd16s(row + 0x84) >= 0) {
        e.h(0x37110, 0, 0, 0, game, 0, 1, (uint32_t)e.rd16s(row + 0x80)); e.edi(0x37110, mon);
        for (uint32_t j = 0; j < 3; j++) {
            const int32_t evn = e.rd16s(row + 0x84 + 2 * j);
            if (evn < 0) break;
            const int32_t k = e.rd16s(row + 0x8A + 2 * j);
            if (k < 0 || k >= 0x32 || !e.rd32(ev + 4 * k)) continue;
            uint32_t w[7] = { game, (uint32_t)evn, skill, lvl, e.rd32(ev + 4 * k), 1, (uint32_t)e.rd16s(row + 0x80) };
            e.h(0x37190, 0, 0, 0, 0, mon, w[0], w[1], w[2], w[3], w[4], w[5], w[6]);   // s4..s6 are not in the hook record
        }
    }
    const int32_t u = e.rd16s(row + 0xE4);
    if (u > 0 && u < 0x2B && mon && e.rd32(mon) == 1 && e.rd32(mon + 0x14)) {
        const uint32_t md = e.rd32(mon + 0x14);
        const uint32_t pp = e.rd32(game + 0xF0 + 4 * e.rd32(md + 0x58));
        if (pp && !(e.rd8(md + 0x16) & 8)) e.wr32(pp + 0x2C8, e.rd32(pp + 0x2C8) + 1);
        if (e.rd32(mon) == 1 && e.rd32(mon + 0x14)) { const uint32_t m2 = e.rd32(mon + 0x14); e.wr8(m2 + 0x16, e.rd8(m2 + 0x16) | 8); }
        for (uint32_t i = 0; i < 9; i++) {
            if (e.rd8(md + 0x1C + i)) continue;
            e.wr8(md + 0x1C + i, (uint8_t)u);
            if (e.rd32(um + 4 * u)) e.h(0x4D420, 0, mon, (uint32_t)u, 0, 0, 1);   // the callback is pinned to a hooked address (generator)
            break;
        }
    }
    const int32_t ov = e.rd16s(row + 0xE6);
    if (ov > 0 && ov < (int32_t)e.rd32(dt + 0xBC0)) e.h(0xA83E, 0, 0, 0, 0, 0, mon, (uint32_t)ov, 0);
    uint32_t il = ilvl;
    if (!il) {
        const int32_t st = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
        int32_t cl = 3 * (int32_t)lvl; if (cl <= 1) cl = 1;
        il = (uint32_t)(cl < st ? cl : st);
    }
    e.h(0x92ED0, lvl, mon, 0, 0, 0, game, unit, il, 0);
    return 1;
}
}
