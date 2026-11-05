// 1.13c D2Game+0xE0040, injectable record lookup. EAX = pointer to a dword that must be 0,
// stack word 0 = value stored at record+0x9C, ret 4. A NULL or non-zero EAX takes the fatal path (messages 0x4BB/0x4BC, _exit(-1)).
#include <cstdint>
struct GE0040Env { uint32_t (*a4c0)(uint32_t p); void (*fatal)(uint32_t msg); };
// 1.13c D2Game+0xE0040
void g_e0040(uint32_t eax, uint32_t value, const GE0040Env& env)
{
    if (eax == 0) { env.fatal(0x4BB); return; }
    if (*(const uint32_t*)(uintptr_t)eax != 0) { env.fatal(0x4BC); return; }
    uint32_t rec = env.a4c0(eax);
    *(uint32_t*)(uintptr_t)(rec + 0x9C) = value;
}
