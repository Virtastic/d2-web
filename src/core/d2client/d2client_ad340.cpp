// 1.13c D2Client+0xAD340 -- ECX = packet. Builds a 13-byte block on the stack from the packet's dwords at +0, +4, +8 and the byte at +0xC and
// hands it to 0x821B0 in EDX; returns what that call returns.
#include "d2client_pktg.h"
namespace d2cl {
uint32_t fad340(GE& e, uint32_t pkt)
{
    uint32_t loc[4] = { e.rd32(pkt), e.rd32(pkt + 4), e.rd32(pkt + 8), e.rd8(pkt + 0xc) };
    const uint32_t p = (uint32_t)(uintptr_t)loc;
    const uint32_t v = e.h(0x821B0, 0, 0, p, 0, 0);
    e.blk(0x821B0, p, 4);
    return v;
}
}
