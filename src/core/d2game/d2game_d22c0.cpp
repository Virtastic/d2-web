// 1.13c D2Game+0xD22C0 -- does the unit have a mode-2 record whose size-0x30 block exists, and does 0xD1A30 accept it; injectable callees.
// EDI = unit, stack word 0 = arg (ret 4), result EAX = 0/1. Callees: 0xA928 (D2Common #10858, ECX = unit, returns the record),
// 0xA9DC (D2Common #11034, 2 stack words, ret 8, returns the block), 0xD1A30 (EAX = unit, ECX = arg, plain ret).
#include <cstdint>
struct GD22C0Env {
    uint32_t (*ord10858)(uint32_t ecx);
    uint32_t (*ord11034)(int32_t a, int32_t b);
    uint32_t (*subd1a30)(uint32_t eax, uint32_t ecx);
};
// 1.13c D2Game+0xD22C0
uint32_t g_d22c0(uint32_t unit, uint32_t arg, const GD22C0Env& env)
{
    if (unit == 0) return 0;
    const uint32_t rec = env.ord10858(unit);
    if (rec == 0) return 0;
    const int16_t n = *(const int16_t*)(uintptr_t)(rec + 0x18);
    if (n <= 0) return 0;
    if (env.ord11034((int32_t)n, 0x30) == 0) return 0;
    if (*(const uint8_t*)(uintptr_t)(rec + 0x14) != 2) return 0;
    return env.subd1a30(unit, arg) != 0 ? 1 : 0;
}
