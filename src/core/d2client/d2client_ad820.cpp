// 1.13c D2Client+0xAD820 -- ECX = packet (+1 byte, +2 word, +4 id dword, +8 word); ESI and EDI at entry carry caller residue in their upper halves.
// Byte 0: 0x21950 (ECX = id, EDI = the +2 word, one stack word = EDI with its low half replaced by the +8 word) and its result returned.
// Otherwise 0x9D630 (EDX = id) answers a record or 0 (returns 0); record+0x14 := the +2 word, record+0x24 := the +8 word, 0x9DA70 (ESI = the
// +2 word in its low half), then 0x772E0 and its result returned.
#include "d2client_pktg.h"
namespace d2cl {
uint32_t fad820(GE& e, uint32_t pkt, uint32_t esiIn, uint32_t ediIn)
{
    const uint32_t w2 = (uint32_t)e.rd16s(pkt + 2) & 0xFFFF, w8 = (uint32_t)e.rd16s(pkt + 8) & 0xFFFF, id = e.rd32(pkt + 4);
    if (e.rd8(pkt + 1) == 0) {
        const uint32_t v = e.h(0x21950, 0, id, 0, 0, 0, (ediIn & ~0xFFFFu) | w8);
        e.j(0x21950, w2);
        return v;
    }
    const uint32_t rec = e.h(0x9D630, 0, 0, id);
    if (!rec) return 0;
    e.wr32(rec + 0x14, w2);
    e.wr32(rec + 0x24, w8);
    e.h(0x9DA70, 0, 0, 0, 0, (esiIn & ~0xFFFFu) | w2);
    return e.h(0x772E0);
}
}
