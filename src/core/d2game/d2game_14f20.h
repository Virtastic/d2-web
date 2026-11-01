// 1.13c D2Game+0x14F20 (rolls a rare item's prefixes and suffixes, the sibling of 0x14BF0): ESI = item, one stack word arg (only
// handed on to 0x13DE0), ret 4. Written from the 1.13c disassembly as a rule over E (see d2game_c310.h).
// Rule: pick = word 0xB37E(item) != 0 ? 0x14210 : 0x14370; maxPfx = pick(item, 1), maxSfx = pick(item, 0); either with a zero low
// word returns 0. 0xB0AE(item, maxPfx), 0xB0A8(item, maxSfx), seed = 0xA5EC(item). 0xAA66(item) == 0x3A: the seed advances and
// n = (low & 1) + 3; else it advances and n = low % 3 + 4. Each slot advances the seed; bit = low & 1. p == 3 takes a suffix, else
// q == 3 a prefix, else bit a suffix, clear bit a prefix. A prefix asks v = word 0xB37E(item) != 0 ? 0x144D0(item, 1, 1, 0, 1, 0, 0)
// : 0x148A0(item, 1, 1, 0, 1, 0); a zero low word skips the slot. The clash test of 0x14BF0 follows (0xA9C4, 0xB3AE); no clash:
// 0xB3C6(item, v, p), p++; a clash retries (0xFB retries per slot) and then 0xB3C6(item, 0, p). A suffix uses announce word 0, 0xA90A
// and 0xB3B4 with q. No prefix and no suffix returns 0. Else 0xA65E(item, 0x10, 0), the announce loop of 0x14BF0 (0xA9D0 for each
// nonnull 0xB3AE of 0xA9C4 then 0xA90A, j = 0..2), 0x13DE0 (EAX = item, stack arg). Result 1.
#pragma once
#include <cstdint>

namespace d2g14f20 {
template <class E> uint32_t RollAffixes(E& e, uint32_t item, uint32_t arg)
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
    const uint32_t kind = e.h(0xAA66, 0, 0, 0, 0, 0, item);
    const uint32_t n = kind == 0x3A ? (adv() & 1) + 3 : adv() % 3 + 4;
    uint32_t p = 0, q = 0;
    for (uint32_t slot = 0; slot < n; slot++) {
        const uint32_t bit = adv() & 1;
        const bool sfx = p == 3 || (q != 3 && bit);
        const uint32_t own = sfx ? 0xA90A : 0xA9C4;
        for (uint32_t tries = 0;; tries++) {
            const bool mag = (e.h(0xB37E, 0, 0, 0, 0, 0, item) & 0xFFFF) != 0;
            const uint32_t v = e.h(mag ? 0x144D0 : 0x148A0, 0, 0, 0, 0, 0, item, 1, 1, 0);
            if (!(v & 0xFFFF)) break;
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
    if (!p && !q) return 0;
    e.h(0xA65E, 0, 0, 0, 0, 0, item, 0x10, 0);
    for (uint32_t j = 0; j < 3; j++) {
        uint32_t r = e.h(0xB3AE, 0, 0, 0, 0, 0, e.h(0xA9C4, 0, 0, 0, 0, 0, item, j) & 0xFFFF);
        if (r) e.h(0xA9D0, 0, 0, 0, 0, 0, 0, 0, item, r);
        r = e.h(0xB3AE, 0, 0, 0, 0, 0, e.h(0xA90A, 0, 0, 0, 0, 0, item, j) & 0xFFFF);
        if (r) e.h(0xA9D0, 0, 0, 0, 0, 0, 0, 0, item, r);
    }
    e.h(0x13DE0, item, 0, 0, 0, 0, arg);
    return 1;
}
}
