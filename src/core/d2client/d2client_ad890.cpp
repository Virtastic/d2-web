// 1.13c D2Client+0xAD890 -- ECX = packet (+1 id dword, +5 word); ESI at entry carries caller residue in its upper half. 0x9D630 (EDX = id) answers a
// record or 0 (returns 0). record+0x18 := the word, sign-extended when bit 15 is set, else zero-extended; 0x9DA70 (ESI = the word in its low half);
// then 0x772E0 and its result returned.
#include "d2client_pktg.h"
namespace d2cl {
uint32_t fad890(GE& e, uint32_t pkt, uint32_t esiIn)
{
    const uint32_t id = e.rd32(pkt + 1), w = (uint32_t)e.rd16s(pkt + 5) & 0xFFFF;
    const uint32_t rec = e.h(0x9D630, 0, 0, id);
    if (!rec) return 0;
    e.wr32(rec + 0x18, (w & 0x8000) ? (uint32_t)(int32_t)(int16_t)w : w);
    e.h(0x9DA70, 0, 0, 0, 0, (esiIn & ~0xFFFFu) | w);
    return e.h(0x772E0);
}
}
