// 1.13c D2Game+0xCE120, injectable callees. Registers: EAX (passed on to 0x78430), ECX, EBX, ESI;
// stack word 0 = EDX for the D2Common call; ret 4. 0x78430 is register-convention (EAX) with 5 stack words, ret 0x14.
#include <cstdint>
struct GCE120Env {
    void (*f78430)(uint32_t eax, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
    uint32_t (*o10329)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1);   // thunk 0xAB6E: fastcall ecx/edx + 2 stack words, ret 8
};
// 1.13c D2Game+0xCE120
void g_ce120(uint32_t eax, uint32_t ecx, uint32_t ebx, uint32_t esi, uint32_t w0, const GCE120Env& env)
{
    env.f78430(eax, 0, 3, ebx, esi, 0);
    env.o10329(ecx, w0, ebx, esi);
}
