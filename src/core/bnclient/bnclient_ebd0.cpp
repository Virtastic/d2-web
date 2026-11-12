// 1.13c Bnclient+0xEBD0 -- ECX = out: stores the module global at image offset 0x1F050 there, then returns by the selector at 0x1F878: 0 gives the global at 0x1EE4C, 1 the one at 0x1EEA0, anything else 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_bnclient_1f050;
__attribute__((weak)) uint32_t g_bnclient_1f878;
__attribute__((weak)) uint32_t g_bnclient_1eea0;
__attribute__((weak)) uint32_t g_bnclient_1ee4c;
namespace bnc {
uint32_t febd0(GE& e, uint32_t out)
{
    e.wr32(out, g_bnclient_1f050);
    if (g_bnclient_1f878 == 0) return g_bnclient_1ee4c;
    if (g_bnclient_1f878 == 1) return g_bnclient_1eea0;
    return 0;
}
}
