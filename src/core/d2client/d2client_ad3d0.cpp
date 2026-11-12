// 1.13c D2Client+0xAD3D0 -- ECX = packet (+1 id dword, +5 signed byte, +6 byte, +7 byte). The record with that id is looked up through the
// table at 0x10A608 (bucket id & 0x7F, chain link +0xE4, id +0xC). A miss returns the id. A record whose type word (+0) is not 0 is fatal
// (the Fog message-and-exit path). Otherwise 0xA0C40 (EBX = record) is called with the signed byte, the +6 byte and the +7 byte; the last
// stack word keeps the upper bytes of the packet pointer (retail stores the byte into the saved ECX slot).
#include "d2client_pktg.h"
namespace d2cl {
uint32_t fad3d0(GE& e, uint32_t pkt)
{
    const uint32_t id = e.rd32(pkt + 1);
    uint32_t v = e.rd8(pkt + 5);
    if (v > 0x80) v -= 0x100;
    const uint32_t b6 = e.rd8(pkt + 6), b7 = e.rd8(pkt + 7);
    const uint32_t rec = recFind(e, g_d2client_10a608, id);
    if (!rec) return id;
    if (e.rd32(rec) != 0) {
        const uint32_t s = e.h(0xBF92, 0, 0, 0, 0, 0, 0x5a);
        e.h(0xBF80, 0, 0, 0, 0, 0, 0x6FB8359Fu, s);
        e.h(0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
        return 0;
    }
    return e.h(0xA0C40, 0, 0, 0, rec, 0, v, b6, (pkt & ~0xFFu) | b7);
}
}
