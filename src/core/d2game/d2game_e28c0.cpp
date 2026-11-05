// 1.13c D2Game+0xE28C0 -- stat-record predicate A, injectable stat getter.
// fastcall ecx = record (byte +0xC is its state), edx = kind, stack words (junk, stat owner, junk), ret 0xC. Result in AL.
#include <cstdint>
struct GE28C0Env { int32_t (*stat)(uint32_t owner, uint32_t id, uint32_t idx); };   // D2Common #10174 via thunk 0xA568
// 1.13c D2Game+0xE28C0
uint32_t g_e28c0(uint32_t rec, uint32_t kind, uint32_t owner, const GE28C0Env& env)
{
    if (env.stat(owner, 0x16, 0) != 0 || kind != 0xFF || *(const uint8_t*)(uintptr_t)(rec + 0xC) != 1)
        return env.stat(owner, 0x16, 0xB) != 0;
    return 1;
}
