// 1.13c D2Client+0xADF20 -- ECX = packet (+1 dword, +5 byte, +6 payload, +0x16 and +0x18 words). Calls 0xA7630 (EAX = the +1 dword, five stack words:
// the dword, the byte, the two words, the pointer to packet+6) and returns its result.
#include "d2client_pktg.h"
namespace d2cl {
uint32_t fadf20(GE& e, uint32_t pkt)
{
    const uint32_t id = e.rd32(pkt + 1);
    uint32_t a[5] = { id, e.rd8(pkt + 5), (uint32_t)e.rd16s(pkt + 0x16) & 0xFFFF, (uint32_t)e.rd16s(pkt + 0x18) & 0xFFFF, pkt + 6 };
    const uint32_t v = e.h(0xA7630, id, 0, 0, 0, 0, a[0], a[1], a[2], a[3], a[4]);
    e.blk(0xA7630, (const uint32_t*)a, 5);
    return v;
}
}
