// 1.13c D2Client+0xAD520 -- ECX = packet. The first 42 bytes are copied to 0x122495 and the byte 0x1223F0 is cleared. When the byte at 0x10F2F3
// is 0: 0x1D930 is called first if the dword 0x1223D8 is 2, then 0x1DA90 gets the dword at 0x123395 and 1. The return value is the last call's
// result, or, with the byte non-zero, EAX at entry with its low byte replaced by that byte.
#include "d2client_pktg.h"
namespace d2cl {
uint32_t fad520(GE& e, uint32_t eaxIn, uint32_t pkt)
{
    const uint8_t gate = g_d2client_10f2f0[3];
    for (uint32_t i = 0; i < 42; i++) g_d2client_122494[1 + i] = (uint8_t)e.rd8(pkt + i);
    g_d2client_1223f0[0] = 0;
    if (gate) return (eaxIn & ~0xFFu) | gate;
    if (gw(g_d2client_1223d8) == 2) e.h(0x1D930);
    const uint32_t a = gw(g_d2client_123394, 1);
    return e.h(0x1DA90, a, 0, 0, 0, 0, a, 1);
}
}
