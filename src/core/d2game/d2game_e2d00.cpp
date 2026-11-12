// 1.13c D2Game+0xE2D00 -- ECX = quest record, EDX = event: the D2Common import 0xA532 of the event's +8 item gives its code. Any code but 0x20333367 is returned as is. For that code the +0x1C counter of the record's +0x18 data steps down; when it reaches zero and the data's first byte is set, AL = the record's +9 byte, and for a non-zero byte with the record's +0xC byte below 5 the data's +0x20 flag is set. The upper bytes of EAX stay those of the code.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe2d00(GE& e, uint32_t q, uint32_t ev)
{
    const uint32_t d = e.rd32(q + 0x18);
    const uint32_t v = e.h(0xA532, 0, 0, 0, 0, 0, e.rd32(ev + 8));
    if (v != 0x20333367) return v;
    const uint32_t c = e.rd32(d + 0x1c) - 1;
    e.wr32(d + 0x1c, c);
    if (c || !e.rd8(d)) return v;
    const uint32_t al = e.rd8(q + 9);
    if (al && e.rd8(q + 0xc) < 5) e.wr8(d + 0x20, 1);
    return (v & 0xFFFFFF00u) | al;
}
}
