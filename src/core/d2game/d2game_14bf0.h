// 1.13c D2Game+0x14BF0 (rolls a magic item's prefixes and suffixes): ESI = item, one stack word rec, ret 4. Written from the 1.13c
// disassembly as a rule over E (see d2game_c310.h).
// Rule: pick = word 0xB37E(item) != 0 ? 0x14210 : 0x14370; maxPfx = pick(item, 1), maxSfx = pick(item, 0) (0xB37E asked before
// each); either with a zero low word returns 0. 0xB0AE(item, maxPfx), 0xB0A8(item, maxSfx), seed = 0xA5EC(item). k = 1, 2, 3, 4
// for rec+0xC > 30, 50, 70; the seed advances, n = max(k, low % 5) (signed). Each of n slots advances the seed; bit = low & 1.
// p (prefixes) == 3 takes a suffix; else q (suffixes) == 3 takes a prefix; else bit picks suffix, clear bit prefix.
// A prefix asks id = rec[0x68 + 4p]: word 0xB37E(item) != 0 ? 0x144D0(item, 1, 1, 0, 1, id, 0) : 0x148A0(item, 1, 1, 0, 1, id) = v.
// For j = 0..2: x = 0xA9C4(item, j), r1 = 0xB3AE(word x); a nonnull r1 asks r2 = 0xB3AE(word v), and word v == word x or
// r2+0x5C == r1+0x5C is a clash. No clash: 0xB3C6(item, v, p), p++. A clash retries the slot (the try counter, reset per slot,
// allows 0xFB retries); once spent, 0xB3C6(item, 0, p) without p++. A suffix is the same with rec[0x74 + 4q], announce word 0,
// 0xA90A in place of 0xA9C4, 0xB3B4 in place of 0xB3C6 and q. Then 0xA65E(item, 0x10, 0); for j = 0..2 a nonnull
// 0xB3AE(word 0xA9C4(item, j)) is announced by 0xA9D0(0, 0, item, r, 0, 0), then the same for 0xA90A. Last 0x13DE0 (EAX = item,
// stack rec). Result 1.
#pragma once
#include <cstdint>

namespace d2g14bf0 {
template <class E> uint32_t RollAffixes(E& e, uint32_t item, uint32_t rec)
{
    auto pickMax = [&](uint32_t flag) {
        const bool mag = (e.h(0xB37E, 0, 0, 0, 0, 0, item) & 0xFFFF) != 0;
        return e.h(mag ? 0x14210 : 0x14370, 0, 0, 0, 0, 0, item, flag);
    };
    const uint32_t maxPfx = pickMax(1), maxSfx = pickMax(0);
    if (!(maxPfx & 0xFFFF) || !(maxSfx & 0xFFFF)) return 0;
    e.h(0xB0AE, 0, 0, 0, 0, 0, item, maxPfx);
    e.h(0xB0A8, 0, 0, 0, 0, 0, item, maxSfx);
    const uint32_t seed = e.h(0xA5EC, 0, 0, 0, 0, 0, item);
    auto adv = [&]() {
        const uint64_t v = (uint64_t)e.rd32(seed) * 0x6AC690C5ull + e.rd32(seed + 4);
        e.wr32(seed, (uint32_t)v); e.wr32(seed + 4, (uint32_t)(v >> 32));
        return (uint32_t)v;
    };
    const int32_t lvl = (int32_t)e.rd32(rec + 0xC);
    int32_t k = lvl > 70 ? 4 : lvl > 50 ? 3 : lvl > 30 ? 2 : 1;
    int32_t n = (int32_t)(adv() % 5);
    if (k > n) n = k;
    uint32_t p = 0, q = 0;
    for (int32_t slot = 0; slot < n; slot++) {
        const uint32_t bit = adv() & 1;
        const bool sfx = p == 3 || (q != 3 && bit);
        const uint32_t own = sfx ? 0xA90A : 0xA9C4;
        for (uint32_t tries = 0;; tries++) {
            const uint32_t id = e.rd32(rec + (sfx ? 0x74 + 4 * q : 0x68 + 4 * p));
            const bool mag = (e.h(0xB37E, 0, 0, 0, 0, 0, item) & 0xFFFF) != 0;
            const uint32_t v = mag ? e.h(0x144D0, 0, 0, 0, 0, 0, item, 1, 1, 0, sfx ? 0 : 1)
                                   : e.h(0x148A0, 0, 0, 0, 0, 0, item, 1, 1, 0, sfx ? 0 : 1);
            (void)id;
            bool clash = false;
            for (uint32_t j = 0; j < 3 && !clash; j++) {
                const uint32_t x = e.h(own, 0, 0, 0, 0, 0, item, j);
                const uint32_t r1 = e.h(0xB3AE, 0, 0, 0, 0, 0, x & 0xFFFF);
                if (!r1) continue;
                const uint32_t r2 = e.h(0xB3AE, 0, 0, 0, 0, 0, v & 0xFFFF);
                if ((v & 0xFFFF) == (x & 0xFFFF) || e.rd32(r2 + 0x5C) == e.rd32(r1 + 0x5C)) clash = true;
            }
            if (!clash) {
                if (sfx) e.h(0xB3B4, 0, 0, 0, 0, 0, item, v, q++);
                else e.h(0xB3C6, 0, 0, 0, 0, 0, item, v, p++);
                break;
            }
            if (tries > 0xFA) {
                if (sfx) e.h(0xB3B4, 0, 0, 0, 0, 0, item, 0, q);
                else e.h(0xB3C6, 0, 0, 0, 0, 0, item, 0, p);
                break;
            }
        }
    }
    e.h(0xA65E, 0, 0, 0, 0, 0, item, 0x10, 0);
    for (uint32_t j = 0; j < 3; j++) {
        uint32_t r = e.h(0xB3AE, 0, 0, 0, 0, 0, e.h(0xA9C4, 0, 0, 0, 0, 0, item, j) & 0xFFFF);
        if (r) e.h(0xA9D0, 0, 0, 0, 0, 0, 0, 0, item, r);
        r = e.h(0xB3AE, 0, 0, 0, 0, 0, e.h(0xA90A, 0, 0, 0, 0, 0, item, j) & 0xFFFF);
        if (r) e.h(0xA9D0, 0, 0, 0, 0, 0, 0, 0, item, r);
    }
    e.h(0x13DE0, item, 0, 0, 0, 0, rec);
    return 1;
}
}
