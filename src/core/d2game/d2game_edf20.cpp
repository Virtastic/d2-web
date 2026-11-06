// 1.13c D2Game+0xEDF20, injectable callees. EDI = argument of 0xA478, stack s0..s4 (ret 0x14); the ESI input is not used.
#include <cstdint>
struct GEDF20Env {
    uint32_t (*a478)(uint32_t edi);
    uint32_t (*b204)(uint32_t v);
    void     (*adfc)(uint32_t v, uint32_t s1);
    void     (*eddb0)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s3, uint32_t s4);
};
// 1.13c D2Game+0xEDF20
void g_edf20(uint32_t edi, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, const GEDF20Env& env)
{
    uint32_t v = env.a478(edi);
    if (env.b204(v) == 0) return;
    env.adfc(v, s1);
    env.eddb0(s2, edi, s0, s3, s4);
}
