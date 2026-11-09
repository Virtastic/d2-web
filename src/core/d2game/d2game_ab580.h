// 1.13c D2Game+0xAB580 (AI think, Shadow Master / assassin minion; D2MOO AITHINK_Fn106_143_ShadowMaster): ECX = pGame, EDX = pUnit,
// stack = pAiTickParam (ret 4; the result is residue). Written from the 1.13c disassembly as a rule over E.
// tp: +0 ai control (+0x14 counter, +0x18 held skill, +0x1C level), +8 target, +0x14 distance, +0x18 combat flag, +0x1C monstats row.
// E supplies dt (the skills table base, [[0xF829C]]) and masks ([0xF8BE0]). Percent rolls are the inline seed step on unit +0x20/+0x24;
// the other rolls are 0x1180 (EAX = range, ECX = &seed). Module addresses are passed as the capture logs them (0xB2000000 | rva).
#pragma once
#include <cstdint>

namespace gab580 {
static constexpr uint32_t kFile = 0xB20FB6B0u, kCb = 0xB20A5230u;
template <class E> uint32_t Step(E& e, uint32_t unit)
{
    const uint64_t t = (uint64_t)e.rd32(unit + 0x20) * 0x6AC690C5ull + e.rd32(unit + 0x24);
    e.wr32(unit + 0x20, (uint32_t)t);
    e.wr32(unit + 0x24, (uint32_t)(t >> 32));
    return (uint32_t)t;
}
template <class E> uint32_t A6ab0(E& e, uint32_t owner, uint32_t skill, uint32_t tgt, uint32_t unit, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3)
{
    const uint32_t v = e.h(0xA6AB0, owner, skill, 0, 0, tgt, s0, s1, s2, s3);
    e.edi(0xA6AB0, unit);
    return v;
}
template <class E> void AiThink(E& e, uint32_t dt, uint32_t masks, uint32_t game, uint32_t unit, uint32_t tp)
{
    // 1.13c D2Game+0xAB580
    const uint32_t ai = e.rd32(tp), comb = e.rd32(tp + 0x18), row = e.rd32(tp + 0x1C), sk = e.rd32(unit + 0xA8);
    uint32_t tgt = e.rd32(tp + 8);
    const int32_t dist = (int32_t)e.rd32(tp + 0x14);
    auto W = [&](uint32_t o) { return e.rd16s(row + o); };
    auto idle = [&](uint32_t n) { e.h(0xF22C0, game, 0, 0, 0, unit, n); };
    auto d1900 = [&](uint32_t o) { return (int32_t)e.h(0x1900, unit, 0, 0, o); };
    auto roll = [&](uint32_t n) { return (int32_t)e.h(0x1180, n, unit + 0x20); };
    if (!sk) { idle(100); return; }
    uint32_t owner = 0;
    if (e.rd32(unit) == 1) {
        const uint32_t pd = e.rd32(unit + 0x14), ac = pd ? e.rd32(pd + 0x28) : 0;
        if (ac && e.rd32(ac + 0x28)) owner = e.h(0xE03A0, e.rd32(ac + 0x30), e.rd32(ac + 0x28), e.rd32(ac + 0x2C));
    }
    for (uint32_t n = e.rd32(unit + 0xDC); n;) {
        const uint32_t next = e.rd32(n + 0x24);
        if (e.rd8(n) == 2) { e.h(0x8E6F0, n); e.edi(0x8E6F0, game); }
        n = next;
    }
    auto f1e140 = [&](uint32_t t) {
        return e.h(0x1E140, 0, 6, 0, 0, owner, game, unit, t, comb);   // REGS `sh`: stack words 4, 5 (0, 6) in the eax/ecx slots
    };
    if (owner) {
        const int32_t r = W(0x60);
        if (d1900(owner) > r * r && f1e140(0)) return;
    }
    if ((int32_t)e.rd32(ai + 0x14) > 0) {
        e.h(0xE07A0, unit, game);
        const uint32_t t = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
        if (t != unit && t) tgt = t;
        if (!tgt) { e.wr32(ai + 0x14, 0); e.wr32(ai + 0x18, 0); }
        else {
            e.wr32(ai + 0x14, e.rd32(ai + 0x14) - 1);
            if (A6ab0(e, owner, e.rd32(ai + 0x18), tgt, unit, game, comb, 0, 0)) return;
        }
    }
    bool pass1;
    if (dist > W(0x5E)) { tgt = 0; pass1 = true; } else pass1 = !tgt;
    const uint32_t cnt = e.rd32(dt + 0xBA0), recs = e.rd32(dt + 0xB98);
    auto rec = [&](uint32_t id) -> uint32_t { return ((int32_t)id >= 0 && (int32_t)id < (int32_t)cnt) ? recs + id * 0x23C : 0; };
    if (pass1) {
        for (uint32_t it = e.h(0xAE0E, 0, 0, 0, 0, 0, sk); it; it = e.h(0xAE02, 0, 0, 0, 0, 0, it)) {
            const uint32_t id = e.h(0xA49C, 0, 0, 0, 0, 0, it, kFile, 0xD73), r = rec(id);
            if (!r) continue;
            const uint32_t ty = e.rd8(r + 0x230);
            if (ty == 6) {
                if (!e.h(0xA3AC, 0, 0, 0, 0, 0, unit) && Step(e, unit) % 100 < 20) e.h(0xADF0, 0, 0, 0, 0, 0, unit, id, 0xFFFFFFFFu);
                continue;
            }
            if (ty != 1) continue;
            const int32_t w80 = e.rd16s(r + 0x80);
            if (w80 <= 0 || e.h(0xA68E, 0, 0, 0, 0, 0, unit, (uint32_t)w80)) continue;
            if (e.h(0xA3A20, 0, 0, 0, 0, 0, unit, (uint32_t)w80) && roll(100) >= 4) continue;
            if (Step(e, unit) % 100 >= 60) continue;
            const uint32_t s = e.h(0xA3D0, 0, unit, id, 0, 0, 0xFFFFFFFFu);
            if (!s) continue;
            uint32_t v;
            if (e.h(0xAB7A, 0, 0, 0, 0, 0, unit, s) == 1 && !comb) {
                v = e.h(0xF2540, 1, 4, unit, 0, 0, 0, 0xF, 0, 0);   // REGS `dh`: ECX (game) unlogged; stack words 4, 5 = 1, 4
            } else {
                const uint32_t e1 = e.h(0xAE1A, 0, 0, 0, 0, 0, s);
                v = e.h(0xF2380, e1, unit, 0, game, 0, id, 0, 0, 0);
            }
            if (v) return;
        }
    }
    uint32_t ot = 0;
    if (owner) {
        e.h(0xE07A0, owner, game);
        const uint32_t x = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(owner + 0x2C));
        if (x != owner && x && !e.h(0xDFA90, x) && e.h(0xE0A60, x, unit, 0, 0, 0, game)) { tgt = x; ot = x; }
        if (d1900(owner) <= 0x90 && f1e140(tgt)) return;
    }
    if (!tgt) { idle(0x19); return; }
    int32_t lvl = (int32_t)e.rd32(ai + 0x1C); if (lvl <= 1) lvl = 1;
    int32_t th = e.rd16s(row + 0x62 + 2 * e.rd8(game + 0x6D)) - 2 * lvl;
    if (th <= 5) th = 5; else if (th >= 100) th = 100;
    if (comb && roll(100) < th && A6ab0(e, owner, 0, tgt, unit, game, comb, 0, 0)) return;
    uint32_t B[10] = { owner, 0, 0x7FFFFFFFu, 0, 0, 0x7FFFFFFFu, 0, 0, 0, 0 };
    e.h(0xB17B0, 1, e.stk(B), game, 0, 0, unit, kCb);
    if (!comb) {
        if (ot) tgt = ot;
        else if (B[4]) tgt = B[4];
        else if (B[9] && d1900(B[9]) < 0x400) tgt = B[9];
        if (!e.h(0xA3C80, 0, 0, 0, 0, tgt, unit)) {
            const uint32_t x = e.h(0x21260, 0, tgt);
            if (x && !e.h(0xDFA90, x) && d1900(x) < 0x400) tgt = x;
        }
    }
    const int32_t life = (int32_t)e.h(0xA718, 0, 0, 0, 0, 0, unit);
    const int32_t dsq = d1900(tgt);
    const bool f4c = W(0x5A) > 0 && e.h(0xADEA, 0, 0, 0, 0, 0, unit);
    const bool f5c = !e.h(0xAD78, 0, 0, 0, 0, 0, unit, tgt, 4);
    const uint32_t B10 = B[4]; const int32_t B20 = (int32_t)B[8], Bc = (int32_t)B[3], B1c = (int32_t)B[7], B8 = (int32_t)B[2], B18 = (int32_t)B[6];
    if (Bc > 3 && roll(0x20) < 2 * Bc) {
        if (owner && d1900(owner) > 0x24) {
            e.h(0xF2540, 1, 0, unit, 0, 0, owner, 0xF, 0, 0);   // REGS `dh`: stack words 4, 5 = 1, 0
            return;
        }
        if (e.h(0xF2C10, tgt, 0, 0, unit, 0, game, 8, 1)) return;
    }
    struct Ent { uint32_t t, sk; int32_t sc; uint32_t c, d; } L[64];
    L[0] = { tgt, 0, 0, 0, 0 };
    uint32_t n = 1; int32_t acc = 0; const uint32_t w5c = (uint32_t)W(0x5C);
    const uint32_t mk = e.rd8(masks + 8);
    for (uint32_t it = e.h(0xAE0E, 0, 0, 0, 0, 0, sk); it; it = e.h(0xAE02, 0, 0, 0, 0, 0, it)) {
        const uint32_t id = e.h(0xA49C, 0, 0, 0, 0, 0, it, kFile, 0xE3D);
        const uint32_t lb = e.h(0xA400, 0, 0, 0, 0, 0, unit, it, 1);
        const uint32_t r = rec(id);
        if (!r) continue;
        int32_t ebp = e.rd16s(r + 0x232) + e.rd16s(r + 0x174) / 4 + (int32_t)lb;
        uint32_t ctgt = tgt; int32_t sc = 0;
        int32_t stat = -1;
        switch (e.rd8(r + 0x1DC)) {
        case 0: stat = 0x24; break; case 1: stat = 0x27; break; case 2: stat = 0x29; break; case 3: stat = 0x25; break;
        case 4: case 0xC: stat = 0x2B; break; case 5: stat = 0x2D; break; default: break;
        }
        const int32_t v = stat < 0 ? 0 : (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, tgt, (uint32_t)stat, 0);
        { const int32_t hi = (int32_t)(((int64_t)(int32_t)0x99999999 * v) >> 32) >> 2; ebp += hi + (int32_t)((uint32_t)hi >> 31); }
        const int32_t w80 = e.rd16s(r + 0x80);
        auto base = [&]() { return roll(w5c) + ebp; };
        auto chargeSkip = [&]() -> bool {   // masks[8] & rec[4] charge count: true = skip
            if (w80 > 0 && e.h(0xA68E, 0, 0, 0, 0, 0, unit, (uint32_t)w80)) {
                const uint32_t p = e.h(0xA5D4, 0, 0, 0, 0, 0, unit, (uint32_t)w80);
                if (p) { const int32_t q = (int32_t)e.h(0xA712, 0, 0, 0, 0, 0, p, (uint32_t)e.rd16s(r + 0x54), 0); acc += q; if (q >= 3) return true; }
            }
            return false;
        };
        auto pen4 = [&]() { const int32_t w = W(0x56); if (dsq > w * w) ebp -= 10; ebp += W(0x58); if (comb || dsq <= 0x19) ebp += 10; };
        auto pen5 = [&]() -> bool {
            if (!f5c) return true;
            if (e.rd16s(r + 0x46) < 0 && e.rd16s(r + 0x48) >= 0) {
                const uint32_t q = e.h(0x1250, (uint32_t)e.rd16s(r + 0x48));
                if (q) { const int32_t m = e.rd16s(q + 0x96) - 1; if (dsq >= m * m) return true; }
            }
            if (B8 <= 0x19) ebp -= 5; if (dsq <= 0x19) ebp -= 5; if (f4c) ebp -= 5;
            return false;
        };
        switch (e.rd8(r + 0x230)) {
        case 1:
            if (w80 > 0 && !e.h(0xA68E, 0, 0, 0, 0, 0, unit, (uint32_t)w80)) break;
            if (B8 <= 0x19) ebp -= 6;
            if (e.h(0xA3A20, 0, 0, 0, 0, 0, unit, (uint32_t)w80)) ebp -= 10; else ebp += 10;
            sc = base(); ctgt = unit; break;
        case 2:
            if (w80 > 0 && e.h(0xA68E, 0, 0, 0, 0, 0, unit, (uint32_t)w80)) break;
            { const int32_t w82 = e.rd16s(r + 0x82); if (w82 > 0 && e.h(0xA68E, 0, 0, 0, 0, 0, tgt, (uint32_t)w82)) break; }
            if (B8 <= 0x19) ebp -= 10;
            sc = base(); break;
        case 3:
            if (B20 > 5) ebp -= 2 * B20;
            if (B8 <= 0x19) ebp -= 7;
            if (B1c < 3) ebp -= 10;
            sc = base() + 3 * B1c - 9; break;
        case 4:
            pen4();
            if (mk & e.rd8(r + 4)) { if (chargeSkip()) break; ebp += W(0x5A); }
            else if (W(0x5A) > 0 && !f4c) ebp -= 10; else ebp += 4 * acc + 3;
            sc = base(); break;
        case 5:
            if (pen5()) break;
            sc = base(); break;
        case 6:
            if (!e.h(0xA3AC, 0, 0, 0, 0, 0, unit)) { if (roll(100) < 20) e.h(0xADF0, 0, 0, 0, 0, 0, unit, id, 0xFFFFFFFFu); }
            else if (roll(100) < 6) e.h(0xADF0, 0, 0, 0, 0, 0, unit, id, 0xFFFFFFFFu);
            break;
        case 7:
            sc = base();
            if (life > 0x42) { sc = 0; break; }
            ctgt = 0;
            if (!owner) { e.h(0x1100, 0, unit); roll(0x28); e.h(0x1100, 0, unit); roll(0x28); }
            else { e.h(0x1100, 0, owner); e.h(0x1100, 0, tgt); e.h(0x1100, 0, owner); e.h(0x1100, 0, tgt); e.h(0x1100, 0, owner); e.h(0x1130, 0, owner); }
            sc += 10; if (life < 0x2D) sc += 10; break;
        case 8: {
            const int32_t a = base();
            if (life > 0x42) break;
            ctgt = unit; sc = 2 * a; if (life < 0x2D) sc *= 2; break;
        }
        case 11:
            if (pen5()) break;
            sc = base() + 3 * B1c; break;
        case 12:
            if (tgt && e.rd32(tgt) == 1) {
                const uint32_t m = e.h(0x1540, tgt);
                if (!m || e.rd8(m + 0xA0 + e.rd8(game + 0x6D)) < 0x19) break;
            }
            pen4();
            if ((mk & e.rd8(r + 4)) && chargeSkip()) break;
            sc = base(); if (life < 0x4B) sc += 8; if (life < 0x32) sc += 12; break;
        case 13:
            ebp += W(0x58);
            if (W(0x5A) > 0 && !f4c) ebp -= 5; else ebp += acc;
            if ((life < 0x32 || Bc > 3) && B10 && B18 < 4 && d1900(B10) > 0x19) { ebp += 0x14; ctgt = B10; sc = base(); break; }
            if (dsq < 0x19) break;
            if (dsq > 0x144) ebp += 10;
            sc = base(); break;
        default: break;
        }
        if (sc > L[n - 1].sc && n < 64) { L[n] = { ctgt, id, sc, 0, 0 }; ++n; }
    }
    for (uint32_t i = n; i-- > 0;) {
        if (!(Step(e, unit) & 3)) continue;
        const Ent& q = L[i];
        const uint32_t r = e.h(0xA754, 0, 0, 0, 0, 0, unit, q.t, 0);
        if (A6ab0(e, owner, q.sk, q.t, unit, game, r, q.c, q.d)) {
            const uint32_t rr = e.h(0x1420, q.sk);
            if (rr && e.rd16s(rr + 0x2E) == 0x13) { e.wr32(ai + 0x18, q.sk); e.wr32(ai + 0x14, 0x19); }
            return;
        }
    }
    if (tgt) {
        const uint32_t r = e.h(0xA754, 0, 0, 0, 0, 0, unit, tgt, 0);
        if (A6ab0(e, owner, 0, tgt, unit, game, r, 0, 0)) return;
    }
    idle(0xF);
}
}
