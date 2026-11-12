// 1.13c D2Client+0xAD7B0 -- ECX = packet (+1 id dword, +5 dword, +9 word). 0x9D630 (EDX = id) answers a record or 0 (returns 0). With a record:
// 0x4FE20 (EAX = the +5 dword); 0x4FD10 (EAX = word, EDX = id, ESI = the +5 dword, one stack word = word); 0x772E0; then, when the +5 dword
// equals the current record's id (record 0x11BBFC + 0xC, -1 without a current record), 0x21BF0 (one stack word = id) and its result returned;
// otherwise the compared id is returned.
#include "d2client_pktg.h"
namespace d2cl {
uint32_t fad7b0(GE& e, uint32_t pkt)
{
    const uint32_t w = (uint32_t)e.rd16s(pkt + 9) & 0xFFFF, other = e.rd32(pkt + 5), id = e.rd32(pkt + 1);
    if (!e.h(0x9D630, 0, 0, id)) return 0;
    e.h(0x4FE20, other);
    e.h(0x4FD10, w, 0, id, 0, other, w);
    e.h(0x772E0);
    const uint32_t cur = gw(g_d2client_11bbfc);
    const uint32_t curId = cur ? e.rd32(cur + 0xc) : 0xFFFFFFFFu;
    if (other == curId) return e.h(0x21BF0, 0, 0, 0, 0, 0, id);
    return curId;
}
}
