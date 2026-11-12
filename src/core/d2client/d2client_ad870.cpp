// 1.13c D2Client+0xAD870 -- ECX = packet (+1 dword, +5 word, +7 word, +0xB word). Calls 0x9E380 (EAX = the +5 word, EDX = the +1 dword, three stack
// words = the +5, +7 and +0xB words) and returns its result.
#include "d2client_pktg.h"
namespace d2cl {
uint32_t fad870(GE& e, uint32_t pkt)
{
    const uint32_t w5 = (uint32_t)e.rd16s(pkt + 5) & 0xFFFF, w7 = (uint32_t)e.rd16s(pkt + 7) & 0xFFFF, wb = (uint32_t)e.rd16s(pkt + 0xb) & 0xFFFF;
    return e.h(0x9E380, w5, 0, e.rd32(pkt + 1), 0, 0, w5, w7, wb);
}
}
