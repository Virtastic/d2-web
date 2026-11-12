// 1.13c Bnclient+0x12E40 -- ECX = callback, EDX = second callback, no stack arguments: marks the news reader active (0x1F638 = 1), stores the two callbacks at 0x1F63C / 0x1F640, frees and zeroes the news item array as the reset does, zeroes 0x1F8E0 and 0x1F8DC, runs the cache loader 0x12A70 and, when it answers non-zero, the notifier 0x12810; then sends frame 0x46 with the dword at 0x1F8E0 as its 4-byte payload. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_bnclient_1f638;
__attribute__((weak)) uint32_t g_bnclient_1f63c;
__attribute__((weak)) uint32_t g_bnclient_1f640;
__attribute__((weak)) uint32_t g_bnclient_1f8d4;
__attribute__((weak)) uint32_t g_bnclient_1f8cc;
__attribute__((weak)) uint32_t g_bnclient_1f8d0;
__attribute__((weak)) uint32_t g_bnclient_1f8e0;
__attribute__((weak)) uint32_t g_bnclient_1f8dc;
namespace bnclient {
uint32_t f12e40(GE& e, uint32_t cb, uint32_t cb2)
{
    g_bnclient_1f638 = 1;
    g_bnclient_1f63c = cb;
    g_bnclient_1f640 = cb2;
    if (g_bnclient_1f8d4) e.h(0x801e, 0, 0, 0, 0, 0, g_bnclient_1f8d4, 0xB201e2e8u, 0xfffffffeu, 0);
    g_bnclient_1f8cc = 0;
    g_bnclient_1f8d0 = 0;
    g_bnclient_1f8d4 = 0;
    g_bnclient_1f8e0 = 0;
    g_bnclient_1f8dc = 0;
    if (e.h(0x12a70)) e.h(0x12810);
    const uint32_t stamp = g_bnclient_1f8e0;
    e.h(0xeb00, 0, 4, 0, 0, 0, 0x46);
    e.edi9(0xeb00, &stamp, 0xF);
    return 1;
}
}
