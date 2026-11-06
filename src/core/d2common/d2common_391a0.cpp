// 1.13c D2Common+0x391a0
// 1.13c D2Common+0x391A0: record lookup in the 0x144-byte table (sgptDataTables +0xBCC pointer, +0xBD4 count). stdcall (a0, a1), ret 8.
// a0 == 0, an index outside [0, count) or a null record gives 0; otherwise the callee at RVA 0x38A80 runs with EBX = a0, EDI = a1 << 16 and
// stack (record), ret 4, and its result is returned. `dt` is the address of the data-tables block (retail reads the global).
#include <cstdint>
#include <cstring>
namespace {
inline uint32_t Rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline void Wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
}

struct D2Common391A0Env { uint32_t (*callee)(uint32_t ebx, uint32_t edi, uint32_t rec); };
extern "C" uint32_t D2Common_391A0_Raw(uint32_t a0, int32_t a1, uint32_t dt, const D2Common391A0Env* env)
{
    if (a0 == 0) return 0;
    if (a1 >= 0 && a1 < (int32_t)Rd32(dt + 0xBD4)) {
        const uint32_t rec = (uint32_t)a1 * 0x144u + Rd32(dt + 0xBCC);
        if (rec != 0) return env->callee(a0, (uint32_t)a1 << 16, rec);
    }
    return 0;
}
