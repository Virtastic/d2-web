// 1.13c Bnclient+0xF320 -- ECX, EDX, no stack arguments: by the protocol mode at 0x1F878. Mode 0 clears 0x1EE4C / 0x1EFCC, calls 0xC960 with the ECX value and returns 1. Mode 1 clears 0x1EEA0 / 0x1F020 and returns what 0x11DC0 answers (the EDX value; ESI = the ECX value). Any other mode returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_bnclient_1f878;
__attribute__((weak)) uint32_t g_bnclient_1eea0;
__attribute__((weak)) uint32_t g_bnclient_1f020;
__attribute__((weak)) uint32_t g_bnclient_1ee4c;
__attribute__((weak)) uint32_t g_bnclient_1efcc;
namespace bnclient {
uint32_t ff320(GE& e, uint32_t c, uint32_t d)
{
    const uint32_t m = g_bnclient_1f878;
    if (m == 0) {
        g_bnclient_1ee4c = 0;
        g_bnclient_1efcc = 0;
        e.h(0xc960, 0, 0, 0, 0, 0, c);
        return 1;
    }
    if (m == 1) {
        g_bnclient_1eea0 = 0;
        g_bnclient_1f020 = 0;
        return e.h(0x11dc0, 0, 0, 0, 0, c, d);
    }
    return 0;
}
}
