// 1.13c D2Game+0xE2AB0 -- stat-record predicate B, injectable stat getter.
// fastcall ecx = record, edx = kind, stack words (junk, stat owner, junk), ret 0xC. Result in AL only (upper bytes are stale).
#include <cstdint>
struct GE2AB0Env { int32_t (*stat)(uint32_t owner, uint32_t id, uint32_t idx); };
// 1.13c D2Game+0xE2AB0
uint32_t g_e2ab0(uint32_t rec, uint32_t kind, uint32_t owner, const GE2AB0Env& env)
{
    if (env.stat(owner, 0x15, 0) != 0) return 0;
    if (kind == 0xFF) return *(const uint8_t*)(uintptr_t)(rec + 0xC) == 1 ? 1 : 0;
    if (kind != 0xF5) return 0;
    if (env.stat(owner, 0x15, 4) == 0) return 0;
    uint32_t sub = *(const uint32_t*)(uintptr_t)(rec + 0x18);
    return *(const uint8_t*)(uintptr_t)(sub + 0xC) == 0 ? 1 : 0;
}
