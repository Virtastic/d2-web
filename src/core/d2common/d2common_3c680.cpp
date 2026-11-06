// 1.13c D2Common+0x3c680
// 1.13c D2Common+0x3C680: stdcall (unit, a1, a2), ret 0xC. ESI = [unit + 0x10]; the callee at RVA 0x81950 runs with ESI and stack (a1, a2), ret 8,
// and its result is returned.
#include <cstdint>
#include <cstring>
namespace {
inline uint32_t Rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline void Wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
}

struct D2Common3C680Env { uint32_t (*callee)(uint32_t esi, uint32_t w0, uint32_t w1); };
extern "C" uint32_t D2Common_3C680_Raw(uint32_t unit, uint32_t a1, uint32_t a2, const D2Common3C680Env* env)
{
    return env->callee(Rd32(unit + 0x10), a1, a2);
}
