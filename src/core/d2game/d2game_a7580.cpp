// 1.13c D2Game+0xA7580 -- sub_A7580, rebuilt from the retail disassembly (ECX = game, EDX = unit, stack ctx, ret 4): the multi-skill caster AI step.
// ctx: +0 pointer to a record (+0x3C receives the room object), +8 target, +0x14 cur, +0x18 flag, +0x1C pointer to the skill row. EBX = game and ESI = unit
// stay live across the callees (0xF2420 reads ESI, 0xF2380 reads EBX). Every callee is an Env function; the seed is the unit's 64-bit word at +0x20.
// 0xF2380 takes EAX = (what EAX held) with the low byte replaced by the row's cast-mode byte: the skill (sign-extended), the roll, the node or the
// request answer, depending on the site.
#include <cstdint>
#include <cstring>

struct D2GA7580Env {
    uint32_t dtBase;   // [[D2Game+0xF829C]] : the table whose word at +0xA80 is read
    void (*s59c50)(uint32_t game);                                            // EAX game
    uint32_t (*a68e)(uint32_t unit, uint32_t w);                               // stdcall 2
    void (*a3ca)(uint32_t unit, uint32_t w, uint32_t z);                       // stdcall 3
    uint32_t (*a3f4)(uint32_t unit, uint32_t skill);                           // ECX unit, EDX skill: the node
    uint32_t (*ae1a)(uint32_t node);                                           // stdcall 1
    uint32_t (*a49c)(uint32_t node, uint32_t file, uint32_t line);             // stdcall 3
    uint32_t (*f2420)(uint32_t unit, uint32_t delay, uint32_t id, uint32_t mode);  // ESI unit, stack delay, id, mode (ret 0xC)
    uint32_t (*f22c0)(uint32_t game, uint32_t w);                               // EAX game, stack w
    uint32_t (*s1180)(uint32_t limit, uint32_t seedPtr);                       // EAX limit, ECX &seed
    uint32_t (*f2d80)(uint32_t target, uint32_t game, uint32_t unit, uint32_t b, uint32_t one);   // EAX target, stack 4
    uint32_t (*a400)(uint32_t unit, uint32_t node, uint32_t one);              // stdcall 3
    uint32_t (*be810)(uint32_t target, uint32_t unit, uint32_t lvl);           // EAX target, stack unit, lvl
    uint32_t (*f2380)(uint32_t eax, uint32_t unit, uint32_t game, uint32_t sk, uint32_t r, uint32_t z0, uint32_t z1);   // EAX, ECX unit, EBX game, stack 4
    uint32_t (*b0840)(uint32_t unit, uint32_t skill);                          // EAX unit, EDX skill
    uint32_t (*b17b0)(uint32_t eax, uint32_t* blk, uint32_t game, uint32_t unit, uint32_t cb);   // EAX 1, ECX blk, EDX game, stack unit, cb
    uint32_t (*s1130)(uint32_t unit);                                          // ECX unit
    uint32_t (*s1100)(uint32_t unit);                                          // ECX unit
    uint32_t (*a478)(uint32_t unit);                                           // stdcall 1
    uint32_t (*f3d20)(uint32_t y, uint32_t cls, uint32_t x, uint32_t z);       // ECX y, EDX cls, stack x, 0
    uint32_t (*adfc)(uint32_t room, uint32_t cls);                             // stdcall 2
    uint32_t (*f1b10)(uint32_t game, uint32_t unit, uint32_t mode);             // AL mode, stack game, unit
    void (*f2540)(uint32_t game, uint32_t unit, uint32_t t);                   // ECX game, EDX unit, stack t, 2, 0, 0, 1, 0
    uint32_t (*fnAddr)(uint32_t rva);
};
static inline uint32_t a58rd(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline uint32_t a58rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
static inline int32_t a58rd16s(uint32_t a) { int16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
static inline uint32_t a58step(uint32_t seed) {   // the unit seed step; returns the new low word
    const uint64_t s = (uint64_t)a58rd(seed) * 0x6AC690C5ull + a58rd(seed + 4);
    std::memcpy((void*)(uintptr_t)seed, &s, 8);
    return (uint32_t)s;
}
// EAX at return: every exit is a tail of a callee, so the step returns that callee's EAX.
extern "C" uint32_t d2g_a7580(const D2GA7580Env* ep, uint32_t game, uint32_t unit, uint32_t ctx) {
    const D2GA7580Env& e = *ep;
    const uint32_t row = a58rd(ctx + 0x1C), flag = a58rd(ctx + 0x18), target = a58rd(ctx + 8);
    const int32_t cur = (int32_t)a58rd(ctx + 0x14);
    const uint32_t seed = unit + 0x20;
    const uint32_t file = e.fnAddr(0xFB6B0u);
    e.s59c50(game);
    if (e.a68e(unit, 0xC)) e.a3ca(unit, 0xC, 0);
    const uint32_t cls = a58rd8(game + 0x6D);
    const int32_t sk0 = a58rd16s(row + 0x170);
    if (target == 0) {
        const uint32_t node = e.a3f4(unit, (uint32_t)sk0);
        if (node && unit && a58rd(unit) == 1 && a58rd(unit + 0x14)) {
            const uint32_t m = a58rd(a58rd(unit + 0x14) + 0x54);
            if (m == 3 || m == 0x13) {
                const uint32_t mode = e.ae1a(node);
                const uint32_t id = e.a49c(node, file, 0x1537);
                return e.f2420(unit, (uint32_t)a58rd16s(row + 0x5C + cls * 2), id, mode);
            }
        }
        return e.f22c0(game, 0x19);
    }
    const uint32_t node0 = e.a3f4(unit, (uint32_t)sk0);
    if (node0 && flag) {
        const uint32_t rem = a58step(seed) % 100u;
        if ((int32_t)rem < a58rd16s(row + 0x56 + cls * 2)) {
            const uint32_t mode = e.ae1a(node0);
            const uint32_t id = e.a49c(node0, file, 0x1544);
            return e.f2420(unit, (uint32_t)a58rd16s(row + 0x5C + cls * 2), id, mode);
        }
    }
    if (cur < a58rd16s(row + 0x6E + cls * 2)) {
        const uint32_t rem = a58step(seed) % 100u;
        if (rem < 0x28) { const uint32_t v = e.f2d80(target, game, unit, 5, 1); if (v) return v; }
    }
    const int32_t sk1 = a58rd16s(row + 0x174);
    if (sk1 > 0) {
        const uint32_t node1 = e.a3f4(unit, (uint32_t)sk1);
        if (node1) {
            const uint32_t rem = a58step(seed) % 100u;
            if ((int32_t)rem < a58rd16s(row + 0x62 + cls * 2)) {
                const uint32_t lvl = e.a400(unit, node1, 1);
                const uint32_t r = e.be810(target, unit, lvl);
                if (r) { const uint32_t v = e.f2380(((uint32_t)sk1 & ~0xFFu) | a58rd8(row + 0x182), unit, game, (uint32_t)sk1, r, 0, 0); if (v) return v; }
            }
        }
    }
    const int32_t sk2 = a58rd16s(row + 0x176);
    bool sk2ok = false;
    uint32_t eaxHold = 0;
    if (sk2 > 0) {
        const uint32_t node2 = e.a3f4(unit, (uint32_t)sk2);
        if (node2) {
            const uint32_t r = e.s1180(100, seed);
            if ((int32_t)r < 0x3C && cur < 0xE) { sk2ok = true; eaxHold = r; }
        }
    }
    if (!sk2ok) {
        const int32_t sk3 = a58rd16s(row + 0x172);
        if (sk3 > 0) {
            const uint32_t rem = a58step(seed) % 100u;
            if ((int32_t)rem < a58rd16s(row + 0x68 + cls * 2) && e.b0840(unit, (uint32_t)sk3)) {
                uint32_t q[6] = { 0, 0x271, 0x32, 0, 0, 0 };
                e.b17b0(1, q, game, unit, e.fnAddr(0x15C40u));
                if (q[4] != 0) {
                    return e.f2380((q[4] & ~0xFFu) | a58rd8(row + 0x181), unit, game, (uint32_t)sk3, q[4], 0, 0);
                }
                const int32_t sk4 = a58rd16s(row + 0x178);
                if (sk4 > 0) {
                    const int32_t d1 = (int32_t)a58rd(e.dtBase + 0xA80);
                    const uint32_t c1 = (d1 > 0x210) ? 0x210u : 0xFFFFFFFFu;
                    const uint32_t y = e.s1130(unit);
                    const uint32_t x = e.s1100(unit);
                    e.a478(unit);
                    if (e.f3d20(y, c1, x, 0)) {
                        const int32_t d2 = (int32_t)a58rd(e.dtBase + 0xA80);
                        const uint32_t c2 = (d2 > 0x1C5) ? 0x1C5u : 0xFFFFFFFFu;
                        const uint32_t room = e.a478(unit);
                        const uint32_t v = e.adfc(room, c2);
                        const uint32_t rec = a58rd(ctx);
                        std::memcpy((void*)(uintptr_t)(rec + 0x3C), &v, 4);
                        return e.f2380(((uint32_t)sk4 & ~0xFFu) | a58rd8(row + 0x184), unit, game, (uint32_t)sk4, target, 0, 0);
                    }
                }
                return e.f1b10(game, unit, 6);
            }
        }
        // second look at the sk2 skill (EAX = its node)
        if (sk2 > 0) {
            const uint32_t node2 = e.a3f4(unit, (uint32_t)sk2);
            if (node2 && cur < 0xE) { sk2ok = true; eaxHold = node2; }
        }
    }
    if (sk2ok) {
        return e.f2380((eaxHold & ~0xFFu) | a58rd8(row + 0x183), unit, game, (uint32_t)sk2, target, 0, 0);
    }
    if ((int32_t)(a58step(seed) % 100u) < 0x3C) e.f2540(game, unit, target);
    return e.f22c0(game, 5);
}
