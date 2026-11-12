// 1.13c D2Client+0xAD6E0 -- ECX = packet (+1 byte = table row, +2 dword = id). 0xA4E20 (EAX = id & 0x7F, ECX = id, EDX = the row's table address at
// 0x10A608 + row * 0x200, one stack word = the row) answers a record or 0 (returns 0). With a record: when a current record 0x11BBFC is set
// 0xC0250 runs; Ordinal 10706 (record+0x2C, 0); the byte 0x11991C := record+4's low byte; the dword 0x11C3F0 := 0; 0xBF850 (ESI = record);
// 0x11BBFC := the record. Returns 0xBF850's result.
#include "d2client_pktg.h"
namespace d2cl {
uint32_t fad6e0(GE& e, uint32_t pkt)
{
    const uint32_t row = e.rd8(pkt + 1), id = e.rd32(pkt + 2);
    const uint32_t rec = e.h(0xA4E20, id & 0x7f, id, kBase + 0x10A608 + (row << 9), 0, 0, row);
    if (!rec) return 0;
    if (gw(g_d2client_11bbfc)) e.h(0xC0250);
    e.h(0xC3C4, 0, 0, 0, 0, 0, e.rd32(rec + 0x2c), 0);
    g_d2client_11991c[0] = (uint8_t)e.rd32(rec + 4);
    gw32(g_d2client_11c3f0, 0);
    const uint32_t v = e.h(0xBF850, 0, 0, 0, 0, rec);
    gw32(g_d2client_11bbfc, rec);
    return v;
}
}
