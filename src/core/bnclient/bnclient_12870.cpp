// 1.13c Bnclient+0x12870 -- cdecl, no arguments: clears the two news callbacks at 0x1F63C and 0x1F640, frees the news item array at 0x1F8D4 when there is one (Storm 403 with the item type name, line -2, flags 0), zeroes the array triple at 0x1F8CC / 0x1F8D0 / 0x1F8D4 and the active flag at 0x1F638. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_bnclient_1f8d4;
__attribute__((weak)) uint32_t g_bnclient_1f63c;
__attribute__((weak)) uint32_t g_bnclient_1f640;
__attribute__((weak)) uint32_t g_bnclient_1f8cc;
__attribute__((weak)) uint32_t g_bnclient_1f8d0;
__attribute__((weak)) uint32_t g_bnclient_1f638;
namespace bnclient {
uint32_t f12870(GE& e, uint32_t a0)
{
    (void)a0;
    g_bnclient_1f63c = 0;
    g_bnclient_1f640 = 0;
    if (g_bnclient_1f8d4) e.h(0x801e, 0, 0, 0, 0, 0, g_bnclient_1f8d4, 0xB201e2e8u, 0xfffffffeu, 0);
    g_bnclient_1f8cc = 0;
    g_bnclient_1f8d0 = 0;
    g_bnclient_1f8d4 = 0;
    g_bnclient_1f638 = 0;
    return 1;
}
}
