// 1.13c D2Game+0x752E0: dispatch through a table entry's +4 handler, else the default 0x750F0. Injectable callees.
// ECX = a (passed through), EDX = rec (pointer or null); two unused stack words (ret 8).
#include <cstdint>
struct G752E0Env {
    uint32_t (*f749a0)(uint32_t eax, uint32_t ecx);                 // regs eax, ecx; returns the entry pointer
    void     (*handler)(uint32_t fn, uint32_t ecx, uint32_t edx);   // call fn with ECX, EDX
    void     (*f750f0)(uint32_t a);                                 // stdcall, 1 word
};
// 1.13c D2Game+0x752E0
void g_752e0(uint32_t ecx, uint32_t edx, const G752E0Env& env)
{
    uint32_t key = edx ? *(const uint32_t*)(uintptr_t)(edx + 0x10) : 0;
    uint32_t ent = env.f749a0(key, edx);
    if (ent != 0) {
        uint32_t fn = *(const uint32_t*)(uintptr_t)(ent + 4);
        if (fn != 0) { env.handler(fn, ecx, edx); return; }
    }
    env.f750f0(ecx);
}
