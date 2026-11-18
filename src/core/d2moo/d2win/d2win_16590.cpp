// 1.13c D2Win+0x16590 -- stdcall(msg): type-7 window; the first child (list head +0x64, next +0x220) whose key byte (+0x218) equals msg[2] and has a handler (+0x204) is tail-called with msg.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2w {
uint32_t f16590(GE& e, uint32_t* msg)
{
    uint32_t* w = (uint32_t*)(uintptr_t)msg[0];
    if (w[0] != 7) abort();
    for (uint32_t c = w[0x19]; c; c = e.rd32(c + 0x220))
        if ((e.rd8(c + 0x218) == (msg[2] & 0xFF)) && e.rd32(c + 0x204))
            return e.hs(e.rd32(c + 0x204) & 0xFFFFFFu, 0, 0, 0, 0, 0, { (uint32_t)(uintptr_t)msg }, 0, 1);
    return 0;
}
}
