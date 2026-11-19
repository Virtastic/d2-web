// 1.13c D2Common+0x394e0
// 1.13c D2Common+0x394E0: register layout EAX = index, ECX = owner, stack word 0 = a 16-bit sub-index (read with movzx), ret 4. Same table as 0x391A0.
// ECX == 0, an index outside [0, count) or a null record gives 0; otherwise the callee at RVA 0x38B00 runs with EBX = ECX, EDI = (index << 16) + sub
// and stack (record), ret 4. The decompile drops the sub-index and the EDI argument.
#include <cstdint>
#include <cstring>
namespace {
inline uint32_t Rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline void Wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
}

struct D2Common394E0Env { uint32_t (*callee)(uint32_t ebx, uint32_t edi, uint32_t rec); };
extern "C" uint32_t D2Common_394E0_Raw(int32_t eax, uint32_t ecx, uint32_t s0, uint32_t dt, const D2Common394E0Env* env)
{
    if (ecx != 0 && eax >= 0 && eax < (int32_t)Rd32(dt + 0xBD4)) {
        const uint32_t rec = (uint32_t)eax * 0x144u + Rd32(dt + 0xBCC);
        if (rec != 0) return env->callee(ecx, ((uint32_t)eax << 16) + (s0 & 0xFFFFu), rec);
    }
    return 0;
}
