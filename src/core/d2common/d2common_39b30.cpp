// 1.13c D2Common+0x39b30
// 1.13c D2Common+0x39B30: stdcall (unit, a1), ret 8. p = [unit + 0x5C]; when p is set and the dword at p + 0x10 is negative the callee at RVA 0x39530
// runs with ECX = p, EDX = a1 << 16 (no stack words, plain ret) and its result is returned; else 0. unit is not null-checked (retail dereferences it).
#include <cstdint>
#include <cstring>
namespace {
inline uint32_t Rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline void Wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
}

struct D2Common39B30Env { uint32_t (*callee)(uint32_t ecx, uint32_t edx); };
extern "C" uint32_t D2Common_39B30_Raw(uint32_t unit, uint32_t a1, const D2Common39B30Env* env)
{
    const uint32_t p = Rd32(unit + 0x5C);
    if (p != 0 && (int32_t)Rd32(p + 0x10) < 0) return env->callee(p, a1 << 16);
    return 0;
}
