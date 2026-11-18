// 1.13c D2Client+0xAD630 -- ECX = packet (+1 word, +3 payload). When the mode dword 0x11C394 is 2 or 3 and the object pointer 0x11BF4C is set, the
// object at [[0x11BF4C] + 0xC] has its virtual method at vtable+8 called (this = the object; payload pointer, the word, the address of a zeroed
// local), then 0x87840 runs. Returns the mode when it is not 2 or 3, 0 without the object, else 0x87840's result.
#include "d2client_pktg.h"
namespace d2cl {
uint32_t fad630(GE& e, uint32_t pkt)
{
    const uint32_t mode = gw(g_d2client_11c394);
    if (mode != 2 && mode != 3) return mode;
    const uint32_t holder = gw(g_d2client_11bf4c);
    if (!holder) return 0;
    const uint32_t obj = e.rd32(holder + 0xc);
    uint32_t loc = 0;
    const uint32_t fn = e.rd32(e.rd32(obj) + 8);
    e.h(fn - kBase, 0, obj, 0, 0, 0, pkt + 3, (uint32_t)e.rd16s(pkt + 1) & 0xFFFF, (uint32_t)(uintptr_t)&loc);
    return e.h(0x87840);
}
}
