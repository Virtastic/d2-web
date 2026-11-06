// 1.13c D2Game+0xB78A0 (retail asm: ECX = ctx (kept in EBP), EDX = unit (kept in ESI), stack w0 = pointer P to a dword holding a pointer Q; ret 4).
// A node of the unit's list (unit+0x14 -> +0x28 -> +0x20: a circular list headed by that dword's pointee, the first node is [head], link at +0, type at +8)
// is looked up by type 10 (the head itself is tested last); its +0xC / +0x10 become A / B when found (else 0 / 0), only for a player unit (type [unit] == 1).
// Then: #10494(unit, 0xC) set -> #10945(unit, 0xC, 0); 0x9D230(ECX = ctx, EDX = unit, stack [P], [Q]); the lookup runs again: no type-10 node (or a guard that
// fails) and A and B both non-zero -> 0x21290(EAX = ctx, ECX = unit, stack &{ 6 dwords: +8 = 10, +0xC = A, +0x10 = B, the rest stack residue });
// finally 0xF22C0(EAX = ctx, ESI = unit, stack 1).
#include "d2game_kmem.h"

namespace d2g_b78a0 {
struct Env {
    D2gMem m;
    uint32_t (*ord10494)(uint32_t unit, uint32_t state);                  // 0xA68E, 2 stack words
    void (*ord10945)(uint32_t unit, uint32_t a, uint32_t b);              // 0xA3CA, 3 stack words
    void (*s9D230)(uint32_t ctx, uint32_t unit, uint32_t s0, uint32_t s1);
    void (*s21290)(uint32_t ctx, uint32_t unit, uint32_t type, uint32_t a, uint32_t b);   // the callee's block is built by the probe
    void (*sF22C0)(uint32_t ctx, uint32_t unit, uint32_t one);
};
// the type-10 node of the unit's list, 0 when the unit has none (or a guard fails: *guard = false)
inline uint32_t find10(const D2gMem& m, uint32_t unit, bool* guard) {
    *guard = false;
    if (!unit || m.r32(unit) != 1) return 0;
    uint32_t pd = m.r32(unit + 0x14); if (!pd) return 0;
    uint32_t x = m.r32(pd + 0x28); if (!x) return 0;
    uint32_t head = m.r32(x + 0x20); if (!head) return 0;
    *guard = true;
    uint32_t n = m.r32(head);
    if (m.r32(n + 8) != 10) {
        while (n != head) { n = m.r32(n); if (m.r32(n + 8) == 10) break; }
    }
    return m.r32(n + 8) == 10 ? n : 0;
}
inline void run(const Env& e, uint32_t ctx, uint32_t unit, uint32_t P) {
    const D2gMem& m = e.m;
    uint32_t q = m.r32(P);
    uint32_t A = 0, B = 0;
    bool g;
    uint32_t n = find10(m, unit, &g);
    if (n) { A = m.r32(n + 0xC); B = m.r32(n + 0x10); }
    if (e.ord10494(unit, 0xC)) e.ord10945(unit, 0xC, 0);
    e.s9D230(ctx, unit, m.r32(P), m.r32(q));
    n = find10(m, unit, &g);
    if (!n && A && B) e.s21290(ctx, unit, 10, A, B);
    e.sF22C0(ctx, unit, 1);
}
}
