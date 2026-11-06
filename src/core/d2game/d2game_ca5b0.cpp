// 1.13c D2Game+0xCA5B0 (size 53, ret 8): ECX = this, EAX = unit, stack table and level.
// this == 0: table[0xA556(level)] + 0x2CC is incremented. Else a type-1 unit with a non-null +0x14
// pointer gets bit 2 set in the byte at pointer+0x5C. Callee 0xA556 is injected (cdecl-free, ret 4).
#include <cstdint>
struct GCa5b0Env { uint32_t (*fa556)(uint32_t s0); };
void g_ca5b0(uint32_t self, uint32_t unit, uint32_t table, uint32_t level, const GCa5b0Env& env) {
    if (self == 0) {
        uint32_t idx = env.fa556(level);
        const uint32_t* t = (const uint32_t*)(uintptr_t)table;
        uint8_t* rec = (uint8_t*)(uintptr_t)t[idx];
        *(uint32_t*)(rec + 0x2cc) += 1;
        return;
    }
    if (unit == 0) return;
    const uint32_t* u = (const uint32_t*)(uintptr_t)unit;
    if (u[0] != 1) return;
    uint32_t p = u[5];
    if (p == 0) return;
    *(uint8_t*)(uintptr_t)(p + 0x5c) |= 2;
}
