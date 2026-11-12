// 1.13c D2Common+0x1B8E0 -- field compiler (ecx = text, edx = out; four stack words): nothing is written for a null out, a null text or an empty text. A text starting with a minus sign or a digit stores its decimal value; any other text is looked up in the link table at image offset 0xA0AA8 through Fog 10217 and stores the index, or 0 when there is no table or no such name.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2common_a0aa8;
namespace d2common {
uint32_t f1b8e0(GE& e, uint32_t ea, uint32_t s, uint32_t out)
{
    if (!out || !s) return ea;
    const uint32_t c = e.rd8(s);
    if (!c) return ea & 0xffffff00u;
    uint32_t v;
    if (c == '-' || (c >= '0' && c <= '9')) {
        v = e.h(0x1ebb, 0, 0, 0, 0, 0, s);
        e.wr32(out, v);
        return v;
    }
    v = g_d2common_a0aa8;
    if (v) {
        v = e.h(0x923a, 0, 0, 0, 0, 0, v, s, 0);
        if ((int32_t)v >= 0) { e.wr32(out, v); return v; }
    }
    e.wr32(out, 0);
    return v;
}
}
