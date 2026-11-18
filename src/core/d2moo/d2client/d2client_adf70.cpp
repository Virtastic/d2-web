// 1.13c D2Client+0xADF70 -- ECX = packet (+1 byte, +2 dword, +6 byte, +7 word, +9 word). 0xC69A (0, the +1 byte) answers a pointer or 0 (returns 0).
// Then 0xA7400 (EAX = the +7 word, ECX = that pointer, EDX = the +2 dword, five stack words: the +6 byte, the +9 word, the +1 byte, 0, 0) answers
// an object or 0 (returns 0). An object whose first dword is 1 goes to 0x6CB30 (ESI = object) and that result is returned; otherwise the object.
#include "d2client_pktg.h"
namespace d2cl {
uint32_t fadf70(GE& e, uint32_t pkt)
{
    const uint32_t b1 = e.rd8(pkt + 1);
    const uint32_t p = e.h(0xC69A, 0, 0, 0, 0, 0, 0, b1);
    if (!p) return 0;
    uint32_t a[5] = { e.rd8(pkt + 6), (uint32_t)e.rd16s(pkt + 9) & 0xFFFF, b1, 0, 0 };
    const uint32_t obj = e.h(0xA7400, (uint32_t)e.rd16s(pkt + 7) & 0xFFFF, p, e.rd32(pkt + 2), 0, 0, a[0], a[1], a[2], a[3], a[4]);
    e.blk(0xA7400, (const uint32_t*)a, 5);
    if (!obj) return 0;
    if (e.rd32(obj) == 1) return e.h(0x6CB30, 0, 0, 0, 0, obj);
    return obj;
}
}
