// 1.13c D2Game+0xF01D0, injectable callee. ESI = unit (or null), EDI = other; plain ret, result in AX.
#include <cstdint>
struct GF01D0Env { uint32_t (*a4c0)(uint32_t edi); };   // ordinal 10920, 1 word (EDI)
// 1.13c D2Game+0xF01D0
uint32_t g_f01d0(uint32_t esi, uint32_t edi, const GF01D0Env& env)
{
    uint32_t ctx = env.a4c0(edi);
    uint32_t id = esi ? *(const uint32_t*)(uintptr_t)(esi + 0xC) : 0xFFFFFFFFu;
    if (edi == esi) return 1;
    uint32_t n = *(const uint32_t*)(uintptr_t)(ctx + 0x38);
    while (n != 0 && *(const uint32_t*)(uintptr_t)n != id) n = *(const uint32_t*)(uintptr_t)(n + 0x10);
    if (n == 0) return 0;
    return *(const uint16_t*)(uintptr_t)(n + 0xC);
}
