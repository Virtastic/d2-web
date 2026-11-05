// 1.13c D2Common+0x344F0: tail thunk, ECX = *(sgptDataTables + 0xDC), stack word 0 = unit, then jmp 0x34080 (thiscall). The callee is injected for replay.
#include <cstdint>
#include <cstring>
typedef uint32_t (*D2Common34080Fn)(uint32_t ecx, uint32_t unit);
extern "C" uint32_t D2Common_344F0_Seam(D2Common34080Fn f, uint32_t pDataTables, uint32_t unit)
{
    uint32_t v;
    memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(pDataTables) + 0xDC), 4);
    return f(v, unit);
}
