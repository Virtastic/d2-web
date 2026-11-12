// 1.13c D2Common+0x7BDB0 -- state pick (ecx = the record): a record already in state 4 is returned as is; otherwise the worker 0x7BC20 picks its state (eax = the record) and, when the record it answers is in state 4 and bit 0 of the byte at +0x8C of its level settings (+0x58, +0x1B4) is set, the worker 0x688B0 is run on it (edi = the record, stack 0) and its answer returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
uint32_t f7bdb0(GE& e, uint32_t u)
{
    if (e.rd8(u + 0x44) == 4) return u;
    const uint32_t r = e.h(0x7bc20, u);
    if (e.rd8(r + 0x44) != 4) return r;
    const uint32_t c = e.rd32(e.rd32(r + 0x58) + 0x1b4);
    if (!(e.rd8(c + 0x8c) & 1)) return r;
    e.ed(r);
    const uint32_t v = e.h(0x688b0, 0, 0, 0, 0, 0, 0);
    e.edi(0x688b0, r);
    return v;
}
}
