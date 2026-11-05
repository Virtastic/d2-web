// 1.13c D2Common+0x4D870 (#10078): stdcall forwarder over 0x4D140 (FindFreeSpot, 9 stdcall words); the callee is injected so a
// replay probe can record what it is handed.
#include <cstdint>
typedef uint32_t (*D2Common4D140Fn)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
extern "C" uint32_t D2Common_4D870_Forward9_Seam(D2Common4D140Fn f, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
    return f(a0, a1, 0, a2, a3, a3, a4, a5, 1);
}
