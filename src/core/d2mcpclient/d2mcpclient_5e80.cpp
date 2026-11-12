// 1.13c D2MCPClient+0x5E80 -- ECX = packet, EDX = length, one unused stack argument, ret 4: an empty packet or a first byte of 0x1A or more returns 0; otherwise the handler table at 0xA968 is indexed by the first byte, a missing handler returns 0 and a present one is called with the same ECX / EDX (EAX = 0) and its answer returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2mcpclient_a968_a26[26];
namespace d2mcpclient {
uint32_t f5e80(GE& e, uint32_t p, uint32_t n, uint32_t a)
{
    (void)a;
    if (!n) return 0;
    const uint32_t b = e.rd8(p);
    if (b >= 0x1a) return 0;
    const uint32_t fn = g_d2mcpclient_a968_a26[b];
    if (!fn) return 0;
    return e.h(fn & 0xffffffu, 0, p, n);
}
}
