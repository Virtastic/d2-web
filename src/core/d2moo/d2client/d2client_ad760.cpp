// 1.13c D2Client+0xAD760 -- ECX = packet (+1 word). When the word is below the count at [sgptDataTables] + 0xA80 (signed compare) 0x6A0C0 is called
// (EBX = the word, one stack word 0) and its result returned; otherwise EAX still holds the import slot's value (the address of D2Common's
// sgptDataTables), which is returned.
#include "d2client_pktg.h"
namespace d2cl {
uint32_t fad760(GE& e, uint32_t pkt)
{
    const uint32_t w = (uint32_t)e.rd16s(pkt + 1) & 0xFFFF;
    const uint32_t slot = gw(g_d2client_ce474);
    const uint32_t tables = e.rd32(slot);
    if ((int32_t)w < (int32_t)e.rd32(tables + 0xa80)) return e.h(0x6A0C0, 0, 0, 0, w, 0, 0);
    return slot;
}
}
