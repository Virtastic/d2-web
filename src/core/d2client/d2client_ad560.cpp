// 1.13c D2Client+0xAD560 -- ECX = packet (+1 id dword, +5 byte that must be 1, +6/+8/+0xA/+0xC words). The record in the table at 0x10A808 with
// that id (a record whose type word is not 1 is fatal) has its sub-record (+0x14) word +0x14 set to the +6 word, three bytes at +0x1C set
// from the +8 word (low, high) and the low byte of the +0xA word, flag bits 4 (when the +0xC word is non-zero) and 8 set in the byte +0x16,
// then 0x20F20 (EBX = record) is called. Every early exit returns the id.
#include "d2client_pktg.h"
namespace d2cl {
uint32_t fad560(GE& e, uint32_t pkt)
{
    const uint32_t id = e.rd32(pkt + 1);
    const uint32_t w8 = (uint32_t)e.rd16s(pkt + 8) & 0xFFFF, w6 = (uint32_t)e.rd16s(pkt + 6) & 0xFFFF;
    const uint32_t wa = (uint32_t)e.rd16s(pkt + 0xa) & 0xFFFF, wc = (uint32_t)e.rd16s(pkt + 0xc) & 0xFFFF;
    if (e.rd8(pkt + 5) != 1) return id;
    const uint32_t rec = recFind(e, g_d2client_10a808, id);
    if (!rec) return id;
    if (e.rd32(rec) != 1) {
        const uint32_t s = e.h(0xBF92, 0, 0, 0, 0, 0, 0x5a);
        e.h(0xBF80, 0, 0, 0, 0, 0, 0x6FB8359Fu, s);
        e.h(0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
        return 0;
    }
    const uint32_t sub = e.rd32(rec + 0x14);
    if (sub) {   // retail writes through address 0 when the sub-record is missing; that path is not reproduced
        wr16(sub + 0x14, w6);
        e.wr8(sub + 0x1c, w8 & 0xFF);
        e.wr8(sub + 0x1d, w8 >> 8);
        e.wr8(sub + 0x1e, wa & 0xFF);
        if (wc) e.wr8(sub + 0x16, e.rd8(sub + 0x16) | 4);
        e.wr8(sub + 0x16, e.rd8(sub + 0x16) | 8);
    }
    return e.h(0x20F20, 0, 0, 0, rec, 0);
}
}
