// 1.13c D2Game+0xE6420, injectable callees. ECX = rec (slot byte at +0x6D), EDX = owner; ret 4. Always returns 0.
#include <cstdint>
struct GE6420Env {
    uint32_t (*a4c0)(uint32_t owner);                            // ordinal 10920, 1 word
    uint32_t (*a568)(uint32_t list, uint32_t id, uint32_t sub);  // 3 words
    void     (*f354f0)(uint32_t ecx, uint32_t al, uint32_t rec); // ECX = owner, AL = 0x10, 1 stack word
};
// 1.13c D2Game+0xE6420
uint32_t g_e6420(uint32_t ecx, uint32_t edx, const GE6420Env& env)
{
    uint32_t tbl = env.a4c0(edx);
    uint32_t list = *(const uint32_t*)(uintptr_t)(tbl + 0x10 + (uint32_t)*(const uint8_t*)(uintptr_t)(ecx + 0x6D) * 4);
    if (env.a568(list, 0x12, 0) == 0 && env.a568(list, 0x12, 0xF) == 0)
        env.f354f0(edx, 0x10, ecx);
    return 0;
}
