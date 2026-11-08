// 1.13c D2Game+0xAAC30 (sub_AAC30): AI think callback, a targeted-skill pre-check and queue. Registers: EAX = cmd (+0 AI control block, +0x1C the
// monstats row), ECX = unit, stack = game (ret 4); the game slot is reused as a local by the compiler (the first 0xB1C90 out value). Written from the
// 1.13c disassembly over the seam environment (see d2game_addc0.cpp). Callees (RVA, regs a c d b s, then stack words): 0xE03A0 (ECX = [ex+0x28], EDX
// = [ex+0x2C], EAX = [ex+0x30]) answers the target; 0xF22C0 (EAX = game, ESI = unit; code), 0xF2380 (EAX = low byte row+0x180, ECX = unit, EBX = game;
// s0 = row+0x170, 0, 0x1100 result, 0x1130 result), 0x8EA80 (ECX = game, EDX = unit; 0, game+0xA8 + n, 0, 0, 0), 0xF20C0 (EAX = game, EBX = unit;
// delay), 0x1E140 (game, unit, a, b, c, 6), 0xB1C90 (EAX = game; unit, st, &out1, &out2), 0xB0E90 / 0xB0A20 (candidate checks), 0xAB86 (distance of
// two units), 0x1CAE0 (EAX = unit; game, target, a, b, c, d), 0x1180 (seeded roll, ECX = seed), 0xF1B10, 0xF2220, 0xF2540, 0xF1D90.
#include "../fn/d2game_gp.h"
namespace d2g {
namespace {
uint32_t outv(GE& e, uint32_t rva, int sidx, int k)
{
    const uint32_t key = ((uint32_t)(0x20 + sidx * 2 + k) << 24) | rva;
    for (const uint32_t* p = e.sc; p[0]; p += 2) if (p[0] == key) return p[1];
    return 0;
}
uint32_t ad(const void* p) { return (uint32_t)(uintptr_t)p; }
int32_t s16(GE& e, uint32_t a) { return (int16_t)(e.rd32(a & ~3u) >> ((a & 2) * 8)); }
}
void faac30(GE& e, uint32_t cmd, uint32_t unit, uint32_t game)
{
    const uint32_t st = e.rd32(cmd), row = e.rd32(cmd + 0x1C);
    const uint32_t b180 = e.rd8(row + 0x180);
    uint32_t tg = 0, pd = 0, ex = 0;
    if (unit && e.rd32(unit) == 1 && (pd = e.rd32(unit + 0x14)) && (ex = e.rd32(pd + 0x28)) && e.rd32(ex + 0x28))
        tg = e.h(0xE03A0, e.rd32(ex + 0x30), e.rd32(ex + 0x28), e.rd32(ex + 0x2C));
    if (!tg) {
        const uint32_t id = e.rd32(st + 0x1C);
        if (id) {
            for (uint32_t n = e.rd32(game + 0x1120 + (id & 0x7F) * 4); n; n = e.rd32(n + 0xE4))
                if (e.rd32(n + 0xC) == id) {
                    const int32_t sk = s16(e, row + 0x170);
                    if (sk < 0) break;
                    const uint32_t r1 = e.h(0x1130, 0, n), r2 = e.h(0x1100, 0, n);
                    e.h(0xF2380, ((uint32_t)sk & ~0xFFu) | b180, unit, 0, game, 0, (uint32_t)sk, 0, r2, r1);
                    e.h(0x8EA80, 0, game, unit, 0, 0, 0, e.rd32(game + 0xA8) + 6, 0, 0);
                    e.h(0xF20C0, game, 0, 0, unit, 0, 8);
                    return;
                }
        }
        e.h(0xF22C0, game, 0, 0, 0, unit, 10);
        return;
    }
    e.wr32(st + 0x1C, e.rd32(tg + 0xC));
    if (e.h(0xA808, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, unit))) {
        if (e.h(0x1E140, 0, 0, 0, 0, 0, game, unit, 0, 0, 0, 6)) return;
        e.h(0xF22C0, game, 0, 0, 0, unit, 0x21);
        return;
    }
    const uint32_t cls = e.rd8(game + 0x6D);
    const int32_t lim = s16(e, row + 0x68 + cls * 2);
    uint32_t slot = game, l24 = 0, l1c = 0, l20 = 0;
    const uint32_t a1 = e.h(0xB1C90, game, 0, 0, 0, 0, unit, st, ad(&slot), ad(&l24));
    slot = outv(e, 0xB1C90, 2, 0); l24 = outv(e, 0xB1C90, 3, 0);
    uint32_t cand = (a1 && (int32_t)slot <= lim) ? a1 : 0;
    slot = cand;
    const uint32_t q = e.h(0xB1C90, game, 0, 0, 0, 0, tg, st, ad(&l1c), ad(&l20));
    bool second = true;
    if (cand) {
        if (e.h(0xB0E90, cand, unit)) second = false;
        else slot = 0;
    }
    if (second && q) {
        if ((int32_t)e.h(0xB0A20, 0, 0, 0, unit, 0, q) < lim && !e.h(0xB0E90, q, unit)) slot = q;
    }
    int32_t pct = 100;
    const int32_t cx = s16(e, row + 0x32);
    if (cx > 0) { pct = s16(e, row + 0x34) * 100 / cx - 100; if (pct >= 100) pct = 100; }
    const int32_t dist = (int32_t)e.h(0xAB86, 0, 0, 0, 0, 0, tg, unit);
    const int32_t sk = s16(e, row + 0x170);
    if (sk >= 0 && dist > 0x32) {
        const uint32_t r1 = e.h(0x1130, 0, tg), r2 = e.h(0x1100, 0, tg);
        e.h(0xF2380, (r2 & ~0xFFu) | b180, unit, 0, game, 0, (uint32_t)sk, 0, r2, r1);
        e.h(0x8EA80, 0, game, unit, 0, 0, 0, e.rd32(game + 0xA8) + 4, 0, 0);
        e.h(0xF20C0, game, 0, 0, unit, 0, 10);
        return;
    }
    if (dist > s16(e, row + 0x6E + e.rd8(game + 0x6D) * 2))
        if (e.h(0x1CAE0, unit, 0, 0, 0, 0, game, tg, 0, 1, 0x64, 0)) return;
    const int32_t far = s16(e, row + 0x62 + e.rd8(game + 0x6D) * 2);
    const uint32_t kind = e.rd32(tg + 0x10);
    if (dist > far) {
        if (kind == 2 || kind == 6) { if (e.h(0x1CAE0, unit, 0, 0, 0, 0, game, tg, 0, 0, 0, 0)) return; }
        if (kind == 3) { if (e.h(0x1CAE0, unit, 0, 0, 0, 0, game, tg, 0, 1, 0x64, 0)) return; }
    }
    if (e.h(0x1E140, 0, 0, 0, 0, 0, game, unit, slot, l24, 1, 6)) return;
    if (!slot) {
        const uint32_t r = e.h(0x1180, 0x64, unit + 0x20);
        if ((int32_t)r >= s16(e, row + 0x5C + e.rd8(game + 0x6D) * 2)) { e.h(0xF22C0, game, 0, 0, 0, unit, 0xF); return; }
        e.h(0xF1B10, (r & ~0xFFu) | 0xA, 0, 0, 0, 0, game, unit);
        return;
    }
    if (l24) {
        e.h(0xF2220, 4, 0, 0, 0, 0, game, slot);
        e.h(0xF20C0, game, 0, 0, unit, 0, (uint32_t)s16(e, row + 0x56 + e.rd8(game + 0x6D) * 2));
        return;
    }
    if ((int32_t)e.h(0xAB86, 0, 0, 0, 0, 0, tg, slot) >= lim) {
        if (dist > 10) e.h(0xF1D90, 8, tg, 0, 0, 0, game, unit, 2, 6);
        else e.h(0xF22C0, game, 0, 0, 0, unit, 0xF);
        return;
    }
    if (pct) e.wr32(e.rd32(e.rd32(unit + 0x14) + 0x2C) + 0x1C, (uint32_t)pct);   // a target answer implies a player unit with its data block
    e.h(0xF2540, 0, game, unit, 0, 0, slot, 0xF, 0, 0, 1, 0);
}
}
