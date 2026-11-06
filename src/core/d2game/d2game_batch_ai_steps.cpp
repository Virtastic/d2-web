// 1.13c D2Game monster AI steps. Registers and stack words follow the retail calls (ECX = game, EDX = unit, stack word 0 = the AI
// context t, ret 4); every callee is an Env seam so a probe can record exactly what retail passes. Pointer fields go through Env::ptr.
#include <cstdint>
#include <cstring>
namespace d2game_aisteps {
using U = uint32_t; using I = int32_t; using P = void*;
static U r(P p, U o) { U v; std::memcpy(&v, (uint8_t*)p + o, 4); return v; }
static I s(P p, U o) { return I(r(p, o)); }
static I h(P p, U o) { int16_t v; std::memcpy(&v, (uint8_t*)p + o, 2); return v; }
static uint8_t b(P p, U o) { return ((uint8_t*)p)[o]; }
static void w(P p, U o, U v) { std::memcpy((uint8_t*)p + o, &v, 4); }
struct Env {
    P (*ptr)(P, U);
    P (*lookup)(U a, U c, U d);                         // E03A0: EAX = record+0x30, ECX = record+0x28, EDX = record+0x2C; returns the monster record
    void (*endAttack)(P game, P unit, P target, I flag); // DEEE0: ECX = game, EDX = unit, stack (target, flag)
    void (*e07a0)(P mon, P game);                       // E07A0: EAX = monster record, ECX = game
    P (*a598)(U v);                                     // A598 (D2Common 10392): stack (value), returns a record
    void (*castAt)(P unit, I skill, P target, I x, I y, I lvl, P game); // F2380: ECX = unit, EBX = game, stack (skill, target, x, y), AL = level
    void (*idle20C0)(P game, P unit, I frames);         // F20C0: EAX = game, EBX = unit, stack frames
    void (*idle)(P game, P unit, I frames);             // F22C0: EAX = game, ESI = unit, stack frames
    void (*modeTarget)(P game, P unit, P target);       // F2220: EAX = 4, EDI = unit, stack (game, target)
    I (*roll)(P unit, I n);                             // 1180: EAX = n, ECX = &unit+0x20
};
// the inline seed step (mul 0x6AC690C5, add hi): unit+0x20/+0x24 hold lo/hi, the result is lo mod 100
static U roll100(P u) {
    uint64_t v = uint64_t(r(u, 0x20)) * 0x6ac690c5u + r(u, 0x24);
    w(u, 0x20, U(v)); w(u, 0x24, U(v >> 32)); return U(v) % 100;
}

// 1.13c D2Game+0x7CB10 (AI_RollGatedSkillState): the monster AI step for gated skills. t: +0 ai, +8 target, +0x14 distance, +0x18 flag, +0x1C monster row.
// ai: +0x18 next-frame stamp, +0x1C state (0 none, 1 first skill cast, 2 second skill cast). Row words are one per difficulty (index game+0x6D).
inline void d2game_7cb10(Env& e, P g, P u, P t) {
    P ai = e.ptr(t, 0), tgt = e.ptr(t, 8), m = e.ptr(t, 0x1c);
    I dist = s(t, 0x14), flag = s(t, 0x18);
    const U idx = U(b(g, 0x6d)) * 2;
    auto prm = [&](U o) { return I(h(m, o + idx)); };
    P rec = nullptr, mon = nullptr;
    if (u && s(u, 0) == 1) { P pd = e.ptr(u, 0x14); if (pd) rec = e.ptr(pd, 0x28); }
    if (rec && r(rec, 0x28)) mon = e.lookup(r(rec, 0x30), r(rec, 0x28), r(rec, 0x2c));
    if (!mon) { e.endAttack(g, u, nullptr, 1); return; }
    const I kind = s(mon, 0x10);
    if (kind == 0xc && roll100(u) < 0x28) {
        e.e07a0(mon, g);
        P x = e.a598(r(mon, 0x2c));
        e.endAttack(g, u, x == mon ? nullptr : x, 1);
        return;
    }
    auto first = [&] {
        e.castAt(u, h(m, 0x170), tgt, 0, 0, b(m, 0x180), g);
        e.idle20C0(g, u, 8);
        w(ai, 0x18, U(prm(0x62) * 0x19 + s(g, 0xa8))); w(ai, 0x1c, 1);
    };
    auto second = [&] {
        e.castAt(u, h(m, 0x172), u, 0, 0, b(m, 0x181), g);
        w(ai, 0x18, U(prm(0x68) * 0x19 + s(g, 0xa8))); w(ai, 0x1c, 2);
    };
    if (h(m, 0x170) >= 0) {
        if (s(ai, 0x1c) == 0) { first(); return; }
        if (s(ai, 0x1c) == 2 && s(ai, 0x18) < s(g, 0xa8)) {
            if (prm(0x74) < dist) { first(); return; }
            if (flag == 0 && e.roll(u, 100) < prm(0x5c)) { first(); return; }
            if (kind == 0xe && e.roll(u, 100) < 0x32) { first(); return; }
        }
    }
    if (h(m, 0x172) >= 0 && s(ai, 0x1c) == 1 && s(ai, 0x18) < s(g, 0xa8)) {
        if (flag != 0 || dist < prm(0x74)) { second(); return; }
        if (kind != 0xe && e.roll(u, 100) < 5) { second(); return; }
    }
    if (s(ai, 0x1c) == 1) { e.idle20C0(g, u, prm(0x6e)); return; }
    if (flag != 0 && e.roll(u, 100) < prm(0x56)) { e.modeTarget(g, u, tgt); return; }
    e.idle(g, u, prm(0x6e));
}
}
