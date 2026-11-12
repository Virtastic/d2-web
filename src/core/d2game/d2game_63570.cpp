// 1.13c D2Game+0x63570 -- ECX = quest record. With the module's trace flag (global 0x111C20) set, the tracer 0xCBE70 runs (EDX = the text at 0xFBEEC, stack = the record's +4 game, 4). A non-zero +9 byte of the record then runs the registration worker 0xDFCC0 (EAX 0, EBX = the callback 0x619B0, EDI = the game, stack 0) and returns its result; a zero byte returns the flag (the tracer's result when it ran) with AL = 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f63570(GE& e, uint32_t q)
{
    uint32_t v = e.rd32(e.B(2));   // the trace flag
    if (v) v = e.h(0xCBE70, 0, 0, 0xB2000000u | 0xfbeec, 0, 0, e.rd32(q + 4), 4);
    if (!e.rd8(q + 9)) return v & ~0xffu;
    v = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0x619b0, 0, 0);
    e.j(0xDFCC0, e.rd32(q + 4));
    return v;
}
}
