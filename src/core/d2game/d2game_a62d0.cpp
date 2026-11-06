// 1.13c D2Game+0xA62D0 -- sub_A62D0, rebuilt from the retail disassembly (EAX = ctx, ECX = unit, stack game, ret 4): the ranged-or-melee AI step.
// ctx+0x1C is the skill row, ctx+8 an optional target. The AI-event list at [game+0x10F4] must hold a type-0x23 node whose +0x18 record has byte +0x11
// set (the walk reads through null when none exists, as retail does); #10494 (unit, 0x36) must be clear. Without ctx+8 the target comes from 0xA4610
// (callback 0xA38C0). The melee flag is #11138, the distance #10522. A skill cast (short +0x170 > 0, 0xB0840, distance < short +0x56 + 2 * cls,
// a 0..99 roll below short +0x5C + 2 * cls) builds the 8-dword request {value, unit, 0, tx + vx, ty + vy, 0x64 << 8, 0, 0} (value = signed byte +0x180;
// the inlined builder 0xA5070 calls #11103 and 0xA4F20 first) for 0xA5E10; otherwise melee rolls against short +0x62 + 2 * cls (0xF2220) and
// the non-melee case walks to the target (0xF2540, 0xF20C0). Every callee is an Env function.
#include <cstdint>
#include <cstring>

struct D2GA62D0Env {
    uint32_t (*a68e)(uint32_t unit, uint32_t k);                                   // #10494 stdcall 2
    uint32_t (*s4610)(uint32_t game, uint32_t unit, uint32_t* pOut, uint32_t* pFlag, uint32_t zero, uint32_t cb);   // ret 0x18
    uint32_t (*a754)(uint32_t unit, uint32_t t, uint32_t zero);                    // #11138 stdcall 3: the melee flag
    uint32_t (*ab86)(uint32_t unit, uint32_t t);                                   // #10522 stdcall 2: the distance
    uint32_t (*b0840)(uint32_t unit, uint32_t skill);                              // EAX unit, EDX skill
    uint32_t (*s1180)(uint32_t limit, uint32_t seedPtr);                           // EAX limit, ECX &seed
    uint32_t (*s1100)(uint32_t unit);                                              // ECX unit
    uint32_t (*s1130)(uint32_t unit);                                              // ECX unit
    uint32_t (*ab80)(uint32_t unit, uint32_t x, uint32_t y);                       // #10304-ish distance, stdcall 3
    void (*a496)(uint32_t unit, uint32_t v);                                       // #11103 stdcall 2
    uint32_t (*s4f20)(uint32_t value, uint32_t unit);                              // EAX value, ECX unit
    uint32_t (*a3f4)(uint32_t unit, uint32_t sk);                                  // ECX unit, EDX skill
    void (*ad18)(uint32_t p, uint32_t one);                                        // stdcall 2
    void (*s5e10)(uint32_t game, uint32_t blk, uint32_t zero);                     // stdcall 3
    void (*f2540)(uint32_t game, uint32_t unit, uint32_t t);                       // ECX game, EDX unit, stack t, 2, 0, 0, 1, 0
    void (*f20c0)(uint32_t game, uint32_t unit, uint32_t w);                       // EAX game, EBX unit, stack w
    void (*f22c0)(uint32_t game, uint32_t w);                                      // EAX game, stack w
    void (*f2220)(uint32_t code, uint32_t unit, uint32_t game, uint32_t t);        // EAX code, EDI unit, stack game, t
    uint32_t (*fnAddr)(uint32_t rva);
};
static inline uint32_t a62rd(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline uint32_t a62rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
static inline int32_t a62rd16s(uint32_t a) { int16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
extern "C" void d2g_a62d0(const D2GA62D0Env* ep, uint32_t ctx, uint32_t unit, uint32_t game) {
    const D2GA62D0Env& e = *ep;
    const uint32_t row = a62rd(ctx + 0x1C);
    uint32_t node = 0;
    if (a62rd(game + 0x10F4)) {
        node = a62rd(a62rd(game + 0x10F4));
        while (node && a62rd(node) != 0x23) node = a62rd(node + 0xF4);
    }
    if (a62rd8(a62rd(node + 0x18) + 0x11) == 0) { e.f22c0(game, 0x19); return; }
    if (e.a68e(unit, 0x36)) { e.f22c0(game, 0x19); return; }
    uint32_t t = a62rd(ctx + 8);
    if (t == 0) {
        uint32_t outA = 0, outB = 0;
        t = e.s4610(game, unit, &outA, &outB, 0, e.fnAddr(0xA38C0u));
        if (t == 0) { e.f22c0(game, 0x19); return; }
    }
    const uint32_t melee = e.a754(unit, t, 0);
    const int32_t dist = (int32_t)e.ab86(unit, t);
    const uint32_t cls = a62rd8(game + 0x6D);
    const int32_t sk = a62rd16s(row + 0x170);
    bool cast = false;
    if (sk > 0 && e.b0840(unit, (uint32_t)sk) && dist < a62rd16s(row + 0x56 + cls * 2)) {
        const int32_t roll = (int32_t)e.s1180(100, unit + 0x20);
        cast = a62rd16s(row + 0x5C + cls * 2) > roll;
    }
    if (!cast) {
        if (melee == 0) { e.f2540(game, unit, t); e.f20c0(game, unit, 10); return; }
        if ((int32_t)e.s1180(100, unit + 0x20) >= a62rd16s(row + 0x62 + cls * 2)) { e.f22c0(game, 0x19); return; }
        e.f2220(e.s1180(2, unit + 0x20) ? 4u : 5u, unit, game, t);
        return;
    }
    const int32_t tx = (int32_t)e.s1100(t);
    const int32_t ty = (int32_t)e.s1130(t);
    const int32_t dx = tx - (int32_t)e.s1100(unit);
    const int32_t dy = ty - (int32_t)e.s1130(unit);
    int32_t d = (int32_t)e.ab80(unit, (uint32_t)tx, (uint32_t)ty);
    if (d == 0) d = 1;
    const int32_t spd = a62rd16s(row + 0x68 + cls * 2);
    const int32_t vx = (int32_t)((uint32_t)spd * (uint32_t)dx) / d;
    const int32_t vy = (int32_t)((uint32_t)spd * (uint32_t)dy) / d;
    const int32_t value = (int8_t)a62rd8(row + 0x180);
    uint32_t buf[8];
    e.a496(unit, 0);
    std::memset(buf, 0, sizeof buf);
    buf[0] = (uint32_t)value; buf[1] = unit;
    e.s4f20((uint32_t)value, unit);   // the builder's flag byte (0x64 or 0x65) is overwritten below
    e.a496(unit, e.a3f4(unit, (uint32_t)a62rd16s(row + 0x170)));
    e.ad18(a62rd(unit + 0x2C), 1);
    buf[3] = (uint32_t)(tx + vx); buf[4] = (uint32_t)(ty + vy);
    buf[5] = 0x64u << 8;
    e.s5e10(game, (uint32_t)(uintptr_t)buf, 0);
}
