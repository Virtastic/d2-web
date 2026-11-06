// 1.13c D2Game+0xA5EB0 -- sub_A5EB0, rebuilt from the retail disassembly (EAX = ctx, ECX = unit, stack game, ret 4): the skill-or-melee AI step.
// ctx+0x1C is the skill row. The AI-event list at [game+0x10F4] must hold a type-0x23 node whose +0x18 record has byte +0x11 set (the
// walk reads through null when none exists, as retail does). A target from 0xA4610 (callback 0xA38C0), the melee flag from #11138 and the
// distance from #10522 pick the skill cast (0xF2120), the approach (0xF2540 + 0xF20C0) or the melee roll (the unit seed step, 0xF2220). Every callee is an Env function.
// The decompiler shows the roll compared against a #11138-row short; the disassembly compares it against the skill row's short at +0x62 + 2 * cls.
#include <cstdint>
#include <cstring>

struct D2GA5EB0Env {
    uint32_t (*s4610)(uint32_t game, uint32_t unit, uint32_t* pOut, uint32_t* pFlag, uint32_t zero, uint32_t cb);   // ret 0x18
    uint32_t (*a754)(uint32_t unit, uint32_t t, uint32_t zero);              // #11138 stdcall 3: the melee flag
    uint32_t (*ab86)(uint32_t unit, uint32_t t);                              // #10522 stdcall 2: the distance
    uint32_t (*s1180)(uint32_t limit, uint32_t seedPtr);                      // EAX limit, ECX &seed
    uint32_t (*b0840)(uint32_t unit, uint32_t skill);                         // EAX unit, EDX skill
    void (*f2120)(uint32_t skill, uint32_t game, uint32_t t);                 // EDI skill, stack game, t, 0, 0
    void (*f2540)(uint32_t game, uint32_t unit, uint32_t t);                  // ECX game, EDX unit, stack t, 2, 0, 0, 1, 0
    void (*f20c0)(uint32_t game, uint32_t unit, uint32_t w);                  // EAX game, EBX unit, stack w
    void (*f22c0)(uint32_t game, uint32_t w);                                // EAX game, stack w
    void (*f2220)(uint32_t code, uint32_t unit, uint32_t game, uint32_t t);   // EAX code, EDI unit, stack game, t
    uint32_t (*fnAddr)(uint32_t rva);
};
static inline uint32_t a5rd(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline uint32_t a5rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
static inline int32_t a5rd16s(uint32_t a) { int16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
extern "C" void d2g_a5eb0(const D2GA5EB0Env* ep, uint32_t ctx, uint32_t unit, uint32_t game) {
    const D2GA5EB0Env& e = *ep;
    const uint32_t row = a5rd(ctx + 0x1C);
    uint32_t node = 0;
    if (a5rd(game + 0x10F4)) {
        node = a5rd(a5rd(game + 0x10F4));
        while (node && a5rd(node) != 0x23) node = a5rd(node + 0xF4);
    }
    if (a5rd8(a5rd(node + 0x18) + 0x11) == 0) { e.f22c0(game, 0x19); return; }
    uint32_t outA = 0, outB = 0;
    const uint32_t t = e.s4610(game, unit, &outA, &outB, 0, e.fnAddr(0xA38C0u));
    if (t == 0) { e.f22c0(game, 0x19); return; }
    const uint32_t melee = e.a754(unit, t, 0);
    const int32_t dist = (int32_t)e.ab86(unit, t);
    const uint32_t cls = a5rd8(game + 0x6D);
    const int32_t sk = a5rd16s(row + 0x170);
    if (sk > 0 && dist < a5rd16s(row + 0x56 + cls * 2)) {
        const int32_t roll = (int32_t)e.s1180(100, unit + 0x20);
        if (roll < a5rd16s(row + 0x5C + cls * 2) && e.b0840(unit, (uint32_t)sk)) { e.f2120((uint32_t)sk, game, t); return; }
    }
    if (melee == 0) { e.f2540(game, unit, t); e.f20c0(game, unit, 10); return; }
    const uint64_t lo = a5rd(unit + 0x20), hi = a5rd(unit + 0x24);
    const uint64_t s = lo * 0x6AC690C5ull + hi;
    std::memcpy((void*)(uintptr_t)(unit + 0x20), &s, 8);
    if ((int32_t)((uint32_t)s % 100u) >= a5rd16s(row + 0x62 + cls * 2)) { e.f22c0(game, 0x19); return; }
    e.f2220(e.s1180(2, unit + 0x20) ? 4u : 5u, unit, game, t);
}
