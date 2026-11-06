// 1.13c D2Common+0x3c6a0
// 1.13c D2Common+0x3C6A0: stdcall (unit, a1, a2, a3, a4), ret 0x14. EAX = [unit + 0x10]; the callee at RVA 0x44D90 runs with EAX and stack
// (a1, a2, a3, a4), ret 0x10, and its result is returned.
#include <cstdint>
#include <cstring>
namespace {
inline uint32_t Rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline void Wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
}

struct D2Common3C6A0Env { uint32_t (*callee)(uint32_t eax, uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3); };
extern "C" uint32_t D2Common_3C6A0_Raw(uint32_t unit, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, const D2Common3C6A0Env* env)
{
    return env->callee(Rd32(unit + 0x10), a1, a2, a3, a4);
}
