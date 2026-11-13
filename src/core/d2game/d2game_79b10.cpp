// 1.13c D2Game+0x79B10 -- ECX game, EDX unit, 2 stack words (ret 8): announce through 0x8EA80, then for a live unit without flag +0xC6 bit 0 whose type/mode pass the filter, 0x77CB0(unit) and on a nonzero answer 0x77A50 and 0x77950 (EDI = unit).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
void f79b10(GE& e, uint32_t game, uint32_t unit, uint32_t a, uint32_t b)
{
    e.hs(0x8EA80, 0, game, unit, 0, 0, {3, e.rd32(game + 0xa8) + 1, 0, a, b});
    if (!unit || (e.rd8(unit + 0xc6) & 1)) return;
    const uint32_t t = e.rd32(unit), m = e.rd32(unit + 0x10);
    if (t == 0) { if (m == 0 || m == 0x11) return; }
    else { if (t != 1 || m == 0 || m == 0xc) return; }
    if (e.h(0x77CB0, unit)) {
        e.ed(unit); e.h(0x77A50, 0); e.edi(0x77A50, unit);
        e.ed(unit); e.h(0x77950, 0); e.edi(0x77950, unit);
    }
}
}
