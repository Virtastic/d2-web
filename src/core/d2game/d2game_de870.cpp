// 1.13c D2Game+0xDE870, injectable callees. ESI = unit, EBX = pass-through, stack word 0 = a0 (ret 4). Types 0 and 1
// only. 0xA3E2(unit, 0xC, 0) gives a value both branches pass on. Type 0: 0xDE6D0 (EAX = unit; stack a0, v, ebx).
// Type 1: unit+0x14, then +0x28, then {+0x28, +0x2C, +0x30} are the registers of 0xE03A0 (ECX, EDX, EAX); when that
// node's first dword is 0, 0xDDCB0 runs (EAX = ebx; stack a0, node, unit, v).
#include <cstdint>
struct GDE870Env {
    uint32_t (*a3e2)(uint32_t unit, uint32_t k, uint32_t z);
    void     (*de6d0)(uint32_t eax, uint32_t s0, uint32_t s1, uint32_t s2);
    uint32_t (*e03a0)(uint32_t eax, uint32_t ecx, uint32_t edx);
    void     (*ddcb0)(uint32_t eax, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
};
static inline uint32_t rdw(uint32_t p) { return *(const uint32_t*)(uintptr_t)p; }
// 1.13c D2Game+0xDE870
void g_de870(uint32_t unit, uint32_t ebx, uint32_t a0, const GDE870Env& env)
{
    if (unit == 0) return;
    const uint32_t t = rdw(unit);
    if (t != 0 && t != 1) return;
    const uint32_t v = env.a3e2(unit, 0xC, 0);
    if (t == 0) { env.de6d0(unit, a0, v, ebx); return; }
    const uint32_t p = rdw(unit + 0x14);
    if (!p) return;
    const uint32_t q = rdw(p + 0x28);
    if (!q) return;
    const uint32_t c = rdw(q + 0x28);
    if (!c) return;
    const uint32_t node = env.e03a0(rdw(q + 0x30), c, rdw(q + 0x2C));
    if (!node) return;
    if (rdw(node) != 0) return;
    env.ddcb0(ebx, a0, node, unit, v);
}
