// 1.13c D2Client+0xADB50 -- ECX = packet (+1 byte, +2 dword, +6 word, +8 dword); EBX at entry carries caller residue in its upper bits. Calls 0x43F90
// (EBX = the byte over EBX's upper bits, EDI = the +6 word, two stack words = the +2 and +8 dwords) and returns its result.
#include "d2client_pktg.h"
namespace d2cl {
uint32_t fadb50(GE& e, uint32_t pkt, uint32_t ebxIn)
{
    const uint32_t v = e.h(0x43F90, 0, 0, 0, (ebxIn & ~0xFFu) | e.rd8(pkt + 1), 0, e.rd32(pkt + 2), e.rd32(pkt + 8));
    e.j(0x43F90, (uint32_t)e.rd16s(pkt + 6) & 0xFFFF);
    return v;
}
}
