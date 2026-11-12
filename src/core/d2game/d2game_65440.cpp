// 1.13c D2Game+0x65440 -- ECX = quest record. A +0x4F byte of 1 in the record's +0x18 data, or a +0xC record byte above 5, returns the data pointer. Otherwise the +0xC byte becomes 3, a trace line is formatted (and, with the module's trace flag 0x111C20 set, sent through the tracer 0xCBE70 with the record's +4 game and 4) and the result of the helper 0x64940 (ESI = record) is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f65440(GE& e, uint32_t q)
{
    const uint32_t d = e.rd32(q + 0x18);
    if (e.rd8(d + 0x4f) == 1 || e.rd8(q + 0xc) > 5) return d;
    e.wr8(q + 0xc, 3);
    if (e.rd32(e.B(2))) e.h(0xCBE70, 0, 0, 0xB3000000u, 0, 0, e.rd32(q + 4), 4);
    return e.h(0x64940, 0, 0, 0, 0, q);
}
}
