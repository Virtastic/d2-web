// 1.13c D2Game+0x53B40: ebx = unit, stack a0..a5 (ret 0x18). When a3 (flag) is set, unit != 0 and *unit == 0: t = #A4C0(unit)[0x5C]; if t != 0 and the buffer size a2 >= t[0]
// (signed), t[0] bytes from t[4] are copied to a1 and t[0] is returned when nonzero. Every other path ends in #537D0(ecx = a1, edx = unit[0x60]; a0, unit, a2, a3, a4, a5)
// and returns its result.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_53b40(uint32_t u, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, const D2Seam* e)
{
    if (a3 && u && !d2rd32(u)) {
        const uint32_t p = e->call(0xA4C0, 0, 0, 0, 0, 0, u);
        const uint32_t t = d2rd32(p + 0x5C);
        if (t && (int32_t)a2 >= (int32_t)d2rd32(t)) {
            const uint32_t n = d2rd32(t), src = d2rd32(t + 4);
            for (uint32_t i = 0; i < n; i++) d2wr8(a1 + i, (uint8_t)d2rd8(src + i));
            if (n) return n;
        }
    }
    return e->call(0x537D0, 0, a1, d2rd32(u + 0x60), 0, 0, a0, u, a2, a3, a4, a5);
}
