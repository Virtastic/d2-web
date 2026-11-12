// 1.13c D2Win+0x10740 -- stdcall(msg): type-2 control message forwarder. msg[0] = control (type 2, else fatal 0x15a).
// Calls the control's handler (+0x34) with msg; a nonzero result steps the state word (+0x50: 3 to 4, 1 to 2), clears +0x4c and stamps +0x48 with GetTickCount.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2w {
uint32_t f10740(GE& e, uint32_t* msg, uint32_t tick)
{
    uint32_t* c = (uint32_t*)(uintptr_t)msg[0];
    if (c[0] != 2) abort();
    if (!c[0xd]) return 0;
    const uint32_t rv = e.hs(c[0xd] & 0xFFFFFFu, 0, 0, 0, 0, 0, { (uint32_t)(uintptr_t)msg }, 0, 1);
    if (rv) {
        if (c[0x14] == 3) c[0x14] = 4;
        else if (c[0x14] == 1) c[0x14] = 2;
        else return rv;
        c[0x13] = 0;
        c[0x12] = tick;
    }
    return rv;
}
}
