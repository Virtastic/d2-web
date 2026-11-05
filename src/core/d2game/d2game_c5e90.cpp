// 1.13c D2Game+0xC5E90 -- release the handle at obj+0xC0 and clear the owner's byte; injectable callees.
// EAX = obj (copied to EDI), stack word 0 = ctx (ret 4). Callees: D2Common #10895 (0xAB92, 1 stack word, ret 4) and 0xDFCC0
// (EAX = 0, EBX = callback RVA 0xC5100, EDI = obj, stack word 0 = 0, ret 4).
#include <cstdint>
struct GC5E90Env {
    void (*ord10895)(uint32_t handle);
    void (*subdfcc0)(uint32_t eax, uint32_t ebx, uint32_t edi);
};
static inline uint32_t g_r32(uint32_t p, uint32_t o) { return *(const uint32_t*)(uintptr_t)(p + o); }
// 1.13c D2Game+0xC5E90
void g_c5e90(uint32_t obj, uint32_t ctx, const GC5E90Env& env)
{
    if (g_r32(obj, 0xc0) == 0) return;
    *(uint8_t*)(uintptr_t)(g_r32(ctx, 0x18) + 2) = 0;
    env.ord10895(g_r32(obj, 0xc0));
    env.subdfcc0(0, 0xC5100, obj);
}
