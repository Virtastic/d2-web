// 1.13c Bnclient+0xF2C0 -- ECX, EDX, one stack argument, ret 4: by the protocol mode at 0x1F878. Mode 0 clears 0x1EE1C / 0x1EF9C, calls 0xC860 (ECX value, the stack argument; EAX = the EDX value) and returns 1. Mode 1 clears 0x1EEB0 / 0x1F030 and returns what 0x11CB0 answers (the EDX value, the stack argument; ESI = the ECX value). Any other mode returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_bnclient_1f878;
__attribute__((weak)) uint32_t g_bnclient_1eeb0;
__attribute__((weak)) uint32_t g_bnclient_1f030;
__attribute__((weak)) uint32_t g_bnclient_1ee1c;
__attribute__((weak)) uint32_t g_bnclient_1ef9c;
namespace bnclient {
uint32_t ff2c0(GE& e, uint32_t c, uint32_t d, uint32_t a)
{
    const uint32_t m = g_bnclient_1f878;
    if (m == 0) {
        g_bnclient_1ee1c = 0;
        g_bnclient_1ef9c = 0;
        e.h(0xc860, d, 0, 0, 0, 0, c, a);
        return 1;
    }
    if (m == 1) {
        g_bnclient_1eeb0 = 0;
        g_bnclient_1f030 = 0;
        return e.h(0x11cb0, 0, 0, 0, 0, c, d, a);
    }
    return 0;
}
}
