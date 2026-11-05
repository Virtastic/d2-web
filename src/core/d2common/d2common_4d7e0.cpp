// 1.13c D2Common+0x4D7E0 (#10654): stdcall forwarder over 0x4D140 (FindFreeSpot, 9 stdcall words); the callee is injected so a
// replay probe can record what it is handed.
#include <cstdint>
typedef uint32_t (*D2Common4D140Fn)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
extern "C" uint32_t D2Common_4D7E0_FindFreeSpotR50_Seam(D2Common4D140Fn f, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    return f(a0, a1, a2, a3, a4, a5, a6, 0x32, 1);
}
