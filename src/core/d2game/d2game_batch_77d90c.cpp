// src/core/d2game/d2game_batch_77d90c.cpp -- 1.13c D2Game.dll AI think row 0x820D0 (AITHINK_CommandRollAttack_820D0).
// Faithful to the disassembly (the decompile loses the EAX/EBX/ESI register arguments and the sixth stack word of 0xF2540).
//
// Retail convention: ECX = game, EDX = unit, stack word 0 = tick param tp (ret 4). tp: +0 ai control, +8 target, +0x14 target
// distance (signed), +0x18 combat flag, +0x1C monster row (words at +0x56.. per difficulty, +0x170 skill, +0x180 level).
// Callees are seams on Env keyed by their D2Game RVA; every pointer field is read through Env::ptr (retail byte offsets,
// 32-bit words). A callee's register arguments are named as retail passes them.
#include <cstdint>
#include <cstring>
namespace d2game_batch77d90c {
using U = uint32_t; using I = int32_t; using P = void*;
static U r(P p, U o) { U v; std::memcpy(&v, (uint8_t*)p + o, 4); return v; }
static I s(P p, U o) { return I(r(p, o)); }
static I h(P p, U o) { int16_t v; std::memcpy(&v, (uint8_t*)p + o, 2); return v; }
static uint8_t b(P p, U o) { return ((uint8_t*)p)[o]; }
static void w(P p, U o, U v) { std::memcpy((uint8_t*)p + o, &v, 4); }
struct Env {
    P (*ptr)(P, U);                                        // the 32-bit pointer field at +o
    U (*getUnit)(U type, P game, U guid);                  // 0xE03A0: EAX = type, ECX = game, EDX = guid, plain ret (a unit pointer)
    void (*alloc212F0)(P game, P unit, P loc);             // 0x212F0: EAX = unit, stack (game, &local), ret 8
    void (*copy21290)(P game, P unit, P loc);              // 0x21290: EAX = game, ECX = unit, stack (&local), ret 4
    U (*free20EC0)(P game, P unit);                        // 0x20EC0: EAX = unit, stack (game), ret 4
    U (*castAt)(U eax, P unit, I skill, P target, P game); // 0xF2380: EAX (AL = level), ECX = unit, EBX = game, stack (skill, target, 0, 0), ret 0x10
    U (*roll1180)(P seed, I n);                            // 0x1180: EAX = n, ECX = &seed, plain ret
    U (*mode)(P game, I mode, P target);                   // 0xF2220: EAX = mode, stack (game, target), ret 8
    U (*idle)(P game, P unit, I frames);                   // 0xF22C0: EAX = game, ESI = unit, stack (frames), ret 4
    U (*f26b0)(P unit, P game, P target);                  // 0xF26B0: EAX = 0, ECX = unit, stack (game, target, 0), ret 0xC
    U (*move)(P game, P unit, const U* st);                // 0xF2540: ECX = game, EDX = unit, six stack words st[0..5], ret 0x18
};

// 1.13c D2Game+0x820D0
inline U sub_820D0(Env& e, P game, P unit, P tp) {
    P ai = e.ptr(tp, 0);                                   // [esp+0x10]
    const U comb = r(tp, 0x18);
    const I cnt = s(tp, 0x14);
    P target = e.ptr(tp, 8);
    P row = e.ptr(tp, 0x1C);
    P seed = unit ? (P)((uint8_t*)unit + 0x20) : (P)0x20;
    const U diff = b(game, 0x6D);
    auto roll = [&]() -> U {                               // seed = lo * 0x6AC690C5 + hi, stored back; value = low word
        const uint64_t v = (uint64_t)r(seed, 0) * 0x6AC690C5ull + r(seed, 4);
        w(seed, 0, (U)v); w(seed, 4, (U)(v >> 32));
        return (U)v;
    };
    auto pd = [&]() -> P { return unit && r(unit, 0) == 1 ? e.ptr(unit, 0x14) : nullptr; };
    auto ai28 = [&]() -> P { P d = pd(); return d ? e.ptr(d, 0x28) : nullptr; };
    auto p2c = [&]() -> P { P d = pd(); return d ? e.ptr(d, 0x2C) : nullptr; };   // written through even when null, as retail does
    bool applied = false;
    P a = ai28();
    if (a && e.ptr(a, 0x20)) applied = r(e.ptr(a, 0x20), 8) == 1;
    else if (cnt < 0x14) {
        U found = 0;
        if (a && e.ptr(a, 0x28)) found = e.getUnit(r(a, 0x30), e.ptr(a, 0x28), r(a, 0x2C));
        if (found == (U)(uintptr_t)unit && I(roll() % 100) < h(row, 0x6E + diff * 2)) {
            U local[4] = {0, 0, 1, 0};                     // local_14 = 1 (word 2); words 0 and 1 are stack residue
            e.alloc212F0(game, unit, local);
            e.copy21290(game, unit, local);
            applied = local[2] == 1;
        }
    }
    if (applied) {
        if (comb && h(row, 0x170) >= 0) {
            const U ea = e.free20EC0(game, unit);
            return e.castAt((ea & ~0xFFu) | b(row, 0x180), unit, h(row, 0x170), target, game);
        }
        P t = p2c();
        w(t, 0x18, 2); w(t, 0x1C, 100);
        const U st[6] = {(U)(uintptr_t)target, 2, 0, 0, 1, 0};
        const U v = e.move(game, unit, st);
        if (v) return v;
        return e.free20EC0(game, unit);
    }
    // 0x226D: no result record applied
    if (!comb) {
        if (r(ai, 0x14)) {
            P t = p2c();
            w(t, 0x18, 2); ((uint8_t*)t)[0x20] = 4;
            const U st[6] = {(U)(uintptr_t)target, 2, 0, 0, 1, 7};
            e.move(game, unit, st);
            const U v = roll();
            if (I(v % 100) > 10) w(ai, 0x14, 0);
            return v / 100;
        }
        const U v = e.f26b0(unit, game, target);
        w(ai, 0x14, 1);
        return v;
    }
    if (I(roll() % 100) >= h(row, 0x56 + diff * 2)) return e.idle(game, unit, h(row, 0x62 + diff * 2));
    if (h(row, 0x170) >= 0 && I(e.roll1180(seed, 100)) < h(row, 0x68 + diff * 2))
        return e.castAt(((U)h(row, 0x170) & ~0xFFu) | b(row, 0x180), unit, h(row, 0x170), target, game);
    return e.mode(game, I(roll() % 100) < h(row, 0x5C + diff * 2) ? 4 : 5, target);
}
}
