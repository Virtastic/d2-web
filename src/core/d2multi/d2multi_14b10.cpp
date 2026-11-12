// 1.13c D2Multi+0x14B10 -- stdcall message handler: calls 0x11510, then one of four builders picked by the two flags at 0x39DE8 and 0x39DE4 (both set 0xD0D0, first only 0xD200, second only 0xD330, neither 0xD460), then 0x8620 with EBX = the record at 0x39DD4, ESI = the flag at 0x39DE4 and EDI = the flag at 0x39DE8. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_39de8;
__attribute__((weak)) uint32_t g_d2multi_39de4;
namespace d2multi {
uint32_t f14b10(GE& e, uint32_t a0)
{
    (void)a0;
    e.h(0x11510);
    const uint32_t a = g_d2multi_39de8, b = g_d2multi_39de4;
    if (a) e.h(b ? 0xd0d0 : 0xd200);
    else e.h(b ? 0xd330 : 0xd460);
    e.ed(g_d2multi_39de8);
    e.h(0x8620, 0, 0, 0, 0xB2039dd4u, g_d2multi_39de4);
    e.edi(0x8620, g_d2multi_39de8);
    return 1;
}
}
