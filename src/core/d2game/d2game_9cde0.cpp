// 1.13c D2Game+0x9CDE0 (MonsterSkillRollAndCast), rebuilt from the retail disassembly.
// ESI = unit, stack (arg0, arg1) = (opaque game-side word, skill-use record), ret 8.
// Gates: unit non-null; byte unit+0xC6 bit 0 clear; type 0 with id (+0x10) not 0 and not 0x11, or type 1 with id not 0 and not 0xC;
// the skills row (*arg1) +8 has bit 0x20; then the 64-bit LCG at unit+0x20 steps once (lo*0x6AC690C5 + hi) and lo % 100 must be below 40.
// The skill id (callee 0x18A0, EAX = unit) is range-checked against the data-table count at +0xA80 (else -1); callee 0x15D0 (EAX = class, EDX = 1)
// gives a flag forced to 1 for ids 10, 0x159, 0x22D. A is the level word from #11027; the 0x47450 out-pointer (&B, &A) rewrites both. 0x1280 takes
// EAX = p, EBX = B, EDI = arg1. Every callee is an Env function. Pointers are 32-bit words read by byte offset.
#include <cstdint>
#include <cstring>

struct D2G9CDE0Env {
    uint32_t gTables;   // address of the word holding the data-table block pointer (D2Game+0xF829C)
    uint32_t (*s18a0)(uint32_t unit);                                   // EAX = unit
    uint32_t (*s15d0)(uint32_t cls, uint32_t one);                      // EAX = class, EDX = 1
    uint32_t (*a718)(uint32_t unit);                                    // #11027, stdcall 1
    uint32_t (*s1180)(uint32_t limit, uint32_t seedPtr);                // EAX = limit, ECX = &seed
    uint32_t (*a478)(uint32_t unit);                                    // #10331, stdcall 1
    uint32_t (*s1130)(uint32_t unit);                                   // ECX = unit
    uint32_t (*s1100)(uint32_t unit);                                   // ECX = unit
    uint32_t (*af28)(uint32_t p, uint32_t b, uint32_t c);               // #10224, stdcall 3
    uint32_t (*s47450)(uint32_t a0, uint32_t p, uint32_t r, uint32_t skill, uint32_t* pB, uint32_t* pA, uint32_t zero);   // stdcall 7
    uint32_t (*s1280)(uint32_t p, uint32_t B, uint32_t arg1);           // EAX = p, EBX = B, EDI = arg1
    uint32_t (*a808)(uint32_t x);                                       // #10057, stdcall 1
    uint32_t (*a68e)(uint32_t unit, uint32_t k);                        // #10494, stdcall 2
    uint32_t (*a42a)(uint32_t unit, uint32_t k, uint32_t z);            // #10973, stdcall 3
    uint32_t (*a3a0)(uint32_t unit);                                    // #10913, stdcall 1
    uint32_t (*a4d2)(uint32_t unit, uint32_t k, uint32_t v, uint32_t z);   // #10551, stdcall 4
    void (*f2380)(uint32_t unit, uint32_t a0, uint32_t w0, uint32_t w1, uint32_t B, uint32_t arg1);   // ECX = unit, EBX = arg0, stack 0xB8, 0, B, arg1
};
static inline uint32_t d9crd(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void d9cwr(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
// 1.13c D2Game+0x9CDE0
extern "C" uint32_t d2g_9cde0(const D2G9CDE0Env* ep, uint32_t unit, uint32_t arg0, uint32_t arg1) {
    const D2G9CDE0Env& e = *ep;
    if (!unit) return 0;
    if ((d9crd(unit + 0xC4) >> 16) & 1) return 0;
    const uint32_t type = d9crd(unit), id = d9crd(unit + 0x10);
    if (type == 0) { if (!id || id == 0x11) return 0; }
    else if (type == 1) { if (!id || id == 0xC) return 0; }
    else return 0;
    if (!(d9crd(d9crd(arg1) + 8) & 0x20)) return 0;
    const uint32_t seedPtr = unit + 0x20;
    {
        const uint64_t m = (uint64_t)d9crd(seedPtr) * 0x6AC690C5u + d9crd(seedPtr + 4);
        d9cwr(seedPtr + 4, (uint32_t)(m >> 32)); d9cwr(seedPtr, (uint32_t)m);
        if ((uint32_t)m % 100u >= 0x28) return 0;
    }
    const uint32_t skill = e.s18a0(unit);
    const uint32_t dt = d9crd(e.gTables);
    const int32_t ebx = ((int32_t)skill < 0 || (int32_t)skill >= (int32_t)d9crd(dt + 0xA80)) ? -1 : (int32_t)skill;
    uint32_t flag = e.s15d0(d9crd(unit + 4), 1);
    if (ebx == 0xA || ebx == 0x159 || ebx == 0x22D) flag = 1;
    uint32_t A = e.a718(unit), B = 0;
    if ((int32_t)A >= 0x1E) { if (flag) return 0; if (d9crd(arg1 + 0x14) >= 0xA) return 0; }
    if ((int32_t)e.s1180(0x64, seedPtr) >= 0xF) return 0;
    const uint32_t p = e.a478(unit);
    const uint32_t r30 = e.s1130(unit), r00 = e.s1100(unit);
    const uint32_t q = e.af28(p, r00, r30);
    if (!e.s47450(arg0, p, q, (uint32_t)ebx, &B, &A, 0)) return 0;
    const uint32_t o = e.s1280(p, B, arg1);
    if (!o) return 0;
    if (e.a808(o)) return 0;
    if ((int32_t)A < 0x1E && (int32_t)e.s1180(0x64, seedPtr) < 0x19 && !e.a68e(unit, 0x34)) {
        int32_t edi = (int32_t)(e.a42a(unit, 0xC, 0) << 8);
        const int32_t ebp = (int32_t)e.a42a(unit, 6, 0);
        const int32_t cap = (int32_t)e.a3a0(unit);
        if (edi + ebp > cap) edi = cap - ebp;
        e.a4d2(unit, 6, (uint32_t)edi, 0);
    }
    e.f2380(unit, arg0, 0xB8, 0, B, arg1);
    return 1;
}
