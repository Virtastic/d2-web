// 1.13c D2Game+0xA7290 -- sub_A7290, rebuilt from the retail disassembly (ECX = game, EDX = unit, stack ctx, ret 4).
// ctx: +8 target, +0x14 cur, +0x18 flag, +0x1C pointer to the skill row. With ctx+8 and ctx+0x18 both set it answers through the 8-dword
// block {4, unit, ctx+8, 0, 0, code << 8, 0, 0} (code 100, or 101 when the unit's monstats-style row [unit+4] has a byte +0x104 that meets
// the mask word at [[0xF8BE0] + 0x10]) handed to 0x75E10; otherwise it resolves a target through 0xB17B0 and moves to it (0xF2380 / 0xF2540),
// or falls back to the skill row's range entry (0xF2D80) or the failure 0xF22C0. Every callee is an Env function.
#include <cstdint>
#include <cstring>

struct D2GA7290Env {
    uint32_t maskBase;   // the value of the module global at D2Game+0xF8BE0 (the base of the bit-mask table; the mask is the word at +0x10)
    void (*a496)(uint32_t unit, uint32_t zero);                               // #11103, stdcall 2
    uint32_t (*s1320)(uint32_t cls);                                          // EAX = [unit+4]; returns the row pointer or 0
    void (*s75e10)(uint32_t game, uint32_t blk, uint32_t one);                // stdcall 3
    uint32_t (*b17b0)(uint32_t loc, uint32_t game, uint32_t unit, uint32_t cb);   // EAX 1, ECX loc, EDX game, stack unit, cb (ret 8)
    uint32_t (*a754)(uint32_t unit, uint32_t t, uint32_t zero);              // #11138, stdcall 3
    void (*f2380)(uint32_t res, uint32_t unit, uint32_t game, uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3);   // EAX res (low byte 8), ECX unit, EBX game
    void (*f2540)(uint32_t game, uint32_t unit, uint32_t t);                  // ECX game, EDX unit, stack t, 2, 0, 0, 1, 0
    void (*f2d80)(uint32_t target, uint32_t game, uint32_t unit, uint32_t b, uint32_t zero);   // EAX ctx+8, stack game, unit, b, 0
    void (*f22c0)(uint32_t game, uint32_t w);                                // EAX game, stack w
    uint32_t (*fnAddr)(uint32_t rva);                                         // the address of the module routine at this RVA (the 0xB17B0 callback)
};
static inline uint32_t a7rd(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline uint32_t a7rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
static inline int32_t a7rd16s(uint32_t a) { int16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
extern "C" void d2g_a7290(const D2GA7290Env* ep, uint32_t game, uint32_t unit, uint32_t ctx) {
    const D2GA7290Env& e = *ep;
    const uint32_t row = a7rd(ctx + 0x1C);
    const int32_t cur = (int32_t)a7rd(ctx + 0x14);
    const uint32_t c8 = a7rd(ctx + 8);
    if (c8 != 0 && a7rd(ctx + 0x18) != 0) {
        e.a496(unit, 0);
        uint32_t blk[8] = {4, unit, c8, 0, 0, 0, 0, 0};
        uint32_t code = 100;
        if (unit) {
            const uint32_t t = e.s1320(a7rd(unit + 4));
            if (t && (a7rd8(t + 0x104) & a7rd(e.maskBase + 0x10))) code = 0x65;
        }
        blk[5] = code << 8;
        e.s75e10(game, (uint32_t)(uintptr_t)blk, 1);
        return;
    }
    uint32_t loc[2] = {0, 0x19};
    const uint32_t t = e.b17b0((uint32_t)(uintptr_t)loc, game, unit, e.fnAddr(0xA4180u));
    if (t != 0) {
        const uint32_t res = e.a754(unit, t, 0);
        if (res != 0) e.f2380((res & ~0xFFu) | 8u, unit, game, 0x12C, t, 0, 0);
        else e.f2540(game, unit, t);
        return;
    }
    const uint32_t cls = a7rd8(game + 0x6D);
    if (cur < a7rd16s(row + 0x56 + cls * 2)) { e.f2d80(c8, game, unit, a7rd8(row + 0x5C + cls * 2), 0); return; }
    e.f22c0(game, 0x19);
}
