// 1.13c D2Game+0xEAB10, injectable callee. EDX = param_2, stack param_3 (ECX input ignored), ret 4.
// Builds a 23-dword request on the stack and hands it to 0x6F930 (ECX = param_3, EDX = block).
#include <cstdint>
struct GEAB10Env { void (*f6f930)(uint32_t ecx, const uint32_t* blk); };
// 1.13c D2Game+0xEAB10
void g_eab10(uint32_t edx, uint32_t p3, const GEAB10Env& env)
{
    uint32_t b[0x17] = {};
    b[0] = 0x8000; b[1] = edx; b[2] = edx;
    b[4] = 299; b[12] = 1; b[0x13] = 100;
    env.f6f930(p3, b);
}
