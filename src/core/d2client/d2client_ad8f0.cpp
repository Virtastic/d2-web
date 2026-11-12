// 1.13c D2Client+0xAD8F0 -- ECX = packet. Calls 0x9E3D0 (EAX = the +3 dword) with nine stack words: the +7 byte, the pointer to packet+8, the words at
// +0x18, +0x1A, +0x1C, +0x1E, +0x20, the pointer to the NUL-terminated name at +0x22 and the pointer just past that NUL; returns its result.
#include "d2client_pktg.h"
namespace d2cl {
uint32_t fad8f0(GE& e, uint32_t pkt)
{
    auto w = [&](uint32_t o) { return (uint32_t)e.rd16s(pkt + o) & 0xFFFF; };
    uint32_t end = pkt + 0x22;
    while (e.rd8(end)) end++;
    uint32_t a[9] = { e.rd8(pkt + 7), pkt + 8, w(0x18), w(0x1a), w(0x1c), w(0x1e), w(0x20), pkt + 0x22, end + 1 };
    const uint32_t v = e.h(0x9E3D0, e.rd32(pkt + 3), 0, 0, 0, 0, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]);
    e.blk(0x9E3D0, (const uint32_t*)a, 9);
    return v;
}
}
