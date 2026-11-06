// 1.13c D2Common+0x3CD80 (#10093): stdcall(pUnit, nArg), ret 8. A null unit returns at once (EAX is left as the caller had it).
// Otherwise EDX = *(pUnit + 0x10) and nArg go to 0x455B0 (EDX register + one stack word, ret 4), a seam; its result is dropped.
#include <cstdint>
#include <cstring>

struct D2Common3CD80Env { void (*call455B0)(uint32_t edx, uint32_t arg); };

extern "C" void D2Common_3CD80_Raw(uint32_t pUnit, uint32_t nArg, const D2Common3CD80Env* env)
{
    if (pUnit == 0) return;
    uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(pUnit + 0x10)), 4);
    env->call455B0(v, nArg);
}
