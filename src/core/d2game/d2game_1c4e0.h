// 1.13c D2Game+0x1C4E0 (the monster skill-or-idle chooser behind AITHINK 0x1C8A0): EAX = pUnit, stack = game, class, X, Y, pSeed (ret 0x14).
// Written from the 1.13c disassembly as a rule over E (see d2game_17cb0.h). dt = sgptDataTables (monstats rows +0xA78, count +0xA80, skills +0xB98, count +0xBA0,
// +0xC4 count), bm = the global bit-mask array behind 0xF8BE0 (+0x10, +0x14). Class outside [0, count) or no row ends.
// lvl = 0xA42A (unit, 0xC, 0); p = 0xA3BE (game+0x70, X, lvl) ends at 0; base = p+0x64, lv = max(lvl - p[0x1C], 0). For the six skill ids at p+0x78 (stop at one
// <= 0 or past the skill count): A[i] = 0; 0xA3D0 (ECX = unit, EDX = id, stack -1) == 0 skips; 0xA400 (unit, that, 1) <= 0 skips; a skill row whose byte +0x230 is 1 with
// a short +0x80 in [0, count 0xC4) skips when 0xA68E (unit, short) != 0; id 0x29 calls 0xA3D0 again and, with Y != 0, 0xA400 (unit, it, 1) / 2 + 4 < 0xAB86 (unit, Y) skips.
// A kept id adds (p[0xA8+4i] * lv) / 4 + p[0x90+4i] to base and stores the running total in A[i]. Then t = a roll of the running total + 1 (none when <= 0; a mask when a
// power of two, else a modulo, from the seed's low word after one step). t < p[0x64] or past A[5] ends in the class tail; otherwise k = first A[k] >= t and the skill at
// p+0x78+4k: <= 0 goes to the tail; if the bit-mask test of its row byte +4 with bm[0x14] is clear, 0xF2380 (AL = byte p+0xC0+k over the id, ECX = unit, EBX = game,
// stack id, Y, 0, 0), else 0xA2100 (ECX = unit, stack id, -1) and idle 0xF22C0 (EAX = game, ESI = unit, stack 0xA).
// Tail by class: 0x10F: 0xF2380 with row +0x170 (short) / +0x180 (byte, over the row address); 0x152, 0x167, 0x230, 0x231: 0xA754 (unit, Y, 0)
// == 0 idles; else 0xA496 (unit, 0), then 0x75E10 (game, &blk, 1) with blk = {4, unit, Y, 0, 0, byte5 = 0x64 or 0x65 (0x1320 (EAX = unit class) record byte +0x104 & bm[0x10])};
// every other class idles with 0xA.
#pragma once
#include <cstdint>

namespace g1c4e0 {
// bias: the low byte of the block holding the monstats rows. The capture aligns it to 256 but the replay's large-blob path does not, so the probe
// passes that low byte and the row-address-over-byte word is formed as if the block were 256-aligned.
template <class E> void Choose(E& e, uint32_t unit, uint32_t game, uint32_t cls, uint32_t X, uint32_t Y, uint32_t S, uint32_t dt, uint32_t bm, uint32_t bias = 0)
{
    // 1.13c D2Game+0x1C4E0
    if ((int32_t)cls < 0 || (int32_t)cls >= (int32_t)e.rd32(dt + 0xA80)) return;
    const uint32_t row = e.rd32(dt + 0xA78) + cls * 0x1A8;
    if (!row) return;
    auto idle = [&]() { e.h(0xF22C0, game, 0, 0, 0, unit, 0xA); };
    const uint32_t lvl = e.h(0xA42A, 0, 0, 0, 0, 0, unit, 0xC, 0);
    const uint32_t p = e.h(0xA3BE, 0, 0, 0, 0, 0, e.rd32(game + 0x70), X, lvl);
    if (!p) return;
    int32_t base = (int32_t)e.rd32(p + 0x64);
    int32_t lv = (int32_t)(lvl - e.rd32(p + 0x1C)); if (lv < 0) lv = 0;
    int32_t A[7] = { 0, 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < 6; i++) {
        const int32_t s = (int32_t)e.rd32(p + 0x78 + 4 * i);
        if (s < 1 || s >= (int32_t)e.rd32(dt + 0xBA0)) break;
        const uint32_t sr = (uint32_t)s * 0x23C + e.rd32(dt + 0xB98);
        if (!sr) continue;
        A[i] = 0;
        const uint32_t r1 = e.h(0xA3D0, 0, unit, (uint32_t)s, 0, 0, 0xFFFFFFFFu);
        if (!r1) continue;
        if ((int32_t)e.h(0xA400, 0, 0, 0, 0, 0, unit, r1, 1) <= 0) continue;
        if (e.rd8(sr + 0x230) == 1) {
            const int32_t di = e.rd16s(sr + 0x80);
            if (di >= 0 && di < (int32_t)e.rd32(dt + 0xC4) && e.h(0xA68E, 0, 0, 0, 0, 0, unit, (uint32_t)di)) continue;
        }
        if (s == 0x29) {
            const uint32_t r1b = e.h(0xA3D0, 0, unit, 0x29, 0, 0, 0xFFFFFFFFu);
            if (Y) {
                const int32_t r2b = (int32_t)e.h(0xA400, 0, 0, 0, 0, 0, unit, r1b, 1);
                if ((int32_t)e.h(0xAB86, 0, 0, 0, 0, 0, unit, Y) > r2b / 2 + 4) continue;
            }
        }
        const int32_t t = (int32_t)e.rd32(p + 0xA8 + 4 * i) * lv;
        base += ((t + ((t >> 31) & 3)) >> 2) + (int32_t)e.rd32(p + 0x90 + 4 * i);
        A[i] = base;
    }
    int32_t n = base + 1;
    uint32_t t = 0;
    if (n > 0) {
        const uint64_t v = (uint64_t)e.rd32(S) * 0x6AC690C5u + e.rd32(S + 4);
        e.wr32(S + 4, (uint32_t)(v >> 32)); e.wr32(S, (uint32_t)v);
        t = (n & (n - 1)) ? (uint32_t)v % (uint32_t)n : (uint32_t)(n - 1) & (uint32_t)v;
    }
    bool tail = (int32_t)t < (int32_t)e.rd32(p + 0x64);
    int k = 0;
    if (!tail && (int32_t)t > A[0]) {
        for (;;) {
            if (k >= 6) { tail = true; break; }
            const int32_t a = A[k + 1]; k++;
            if ((int32_t)t <= a) { if (k >= 6) tail = true; break; }
        }
    }
    if (!tail) {
        const uint32_t id = e.rd32(p + 0x78 + 4 * k);
        if ((int32_t)id > 0) {
            const uint32_t sr = id < e.rd32(dt + 0xBA0) ? id * 0x23C + e.rd32(dt + 0xB98) : 0;
            if (!(e.rd8(bm + 0x14) & e.rd8(sr + 4))) {
                e.h(0xF2380, (id & ~0xFFu) | e.rd8(p + k + 0xC0), unit, 0, game, 0, id, Y, 0, 0);
                return;
            }
            e.h(0xA2100, 0, unit, 0, 0, 0, id, 0xFFFFFFFFu);
            idle(); return;
        }
    }
    if (cls > 0x167) { if (cls < 0x230 || cls > 0x231) { idle(); return; } }
    else if (cls != 0x167) {
        if (cls == 0x10F) {
            e.h(0xF2380, ((((row - bias) & ~0xFFu) | e.rd8(row + 0x180)) + bias), unit, 0, game, 0, (uint32_t)e.rd16s(row + 0x170), Y, 0, 0);
            return;
        }
        if (cls != 0x152) { idle(); return; }
    }
    if (!e.h(0xA754, 0, 0, 0, 0, 0, unit, Y, 0)) { idle(); return; }
    e.h(0xA496, 0, 0, 0, 0, 0, unit, 0);
    uint32_t blk[8] = { 4, unit, Y, 0, 0, 0, 0, 0 };
    uint32_t bl = 0x64;
    if (unit) {
        const uint32_t rec = e.h(0x1320, e.rd32(unit + 4));
        if (rec && (e.rd8(rec + 0x104) & e.rd32(bm + 0x10))) bl = 0x65;
    }
    blk[5] = bl << 8;
    e.h(0x75E10, 0, 0, 0, 0, 0, game, e.stk(blk), 1);
    e.blk(0x75E10, blk, 8);
}
}
