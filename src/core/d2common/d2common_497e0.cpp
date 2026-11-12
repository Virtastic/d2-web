// 1.13c D2Common+0x497E0 -- ECX, EDX = owner unit or NULL and seven stack arguments, ret 28 (first stack argument = the item unit: NULL, a type other than 4 or no item data are fatal at lines 0x1505, 0x1506, 0x1508; second = property record; returns the third stack argument). When the item data word at +0x30 is zero: draws through 0x463F0 (record max, record min, item) and calls 0x46E50(0x11); a non-zero draw resolves the stat list through 0x47020 (none is fatal, line 0x29D) and writes stat 0x11 with layer 0 through 0x3A330 (with the fourth stack argument set the value is the negated 0x38B00 answer for stat record 0x11, EDI = 0x110000, 0 when the table has none); the same again with a second draw for stat 0x12. Otherwise (a NULL record is fatal, line 0x3B5): draws through 0x464D0, looks the item record up through 0x719A0 by the class of the owner when it is an item, else of the item itself (none is fatal, line 0x3C0), and takes the larger of the record bytes +0xFF and +0x103 as the base damage. When 0x24430(item, 0x2D) answers zero, or base * draw / 100 is above zero, the draw goes to 0x47EF0 for stats 0x11 and 0x12 (EAX = 1); otherwise 0x47EF0 runs with value 1 and EAX = 0 for stat 0x16 when record byte +0xFE is set, 0x18 when +0x102 is set and 0xA0 when 0x24C30(item) answers non-zero.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2common_99e1c;
namespace d2common {
uint32_t f497e0(GE& e, uint32_t c, uint32_t d, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6)
{
    auto fatal = [&](uint32_t line) { const uint32_t v = e.h(0x921c); e.h(0x9216, 0, 0, 0, 0, 0, 0xB208a728u, v, line); e.h(0x1b0d, 0, 0, 0, 0, 0, 0xffffffffu); };
    (void)s6;
    if (!s0) { fatal(0x1505); return 0; }
    if (e.rd32(s0) != 4) { fatal(0x1506); return 0; }
    const uint32_t idata = e.rd32(s0 + 0x14);
    if (!idata) { fatal(0x1508); return 0; }
    // 0x47EF0: EAX = flag, ECX = item, EDI = stat, six stack words (ECX in, owner, value, s3, s4, s5)
    auto apply = [&](uint32_t flag, uint32_t stat, uint32_t value) {
        const uint32_t w[2] = { s4, s5 };
        e.ed(stat);
        e.h(0x47ef0, flag, s0, 0, 0, 0, c, d, value, s3);
        e.x(0x47ef0, w, 2);
        e.edi(0x47ef0, stat);
    };
    if ((e.rd32(idata + 0x30) & 0xffff) != 0) {
        if (!s1) { fatal(0x3b5); return 0; }
        const uint32_t v = e.h(0x464d0, e.rd32(s1 + 0xc), e.rd32(s1 + 8), s0);
        const uint32_t t = (d && e.rd32(d) == 4) ? e.h(0x719a0, 0, 0, 0, 0, 0, e.rd32(d + 4)) : e.h(0x719a0, 0, 0, 0, 0, 0, e.rd32(s0 + 4));
        if (!t) { fatal(0x3c0); return 0; }
        uint32_t base = e.rd8(t + 0x103);
        if (base <= e.rd8(t + 0xff)) base = e.rd8(t + 0xff);
        if (e.h(0x24430, 0, 0, 0, 0, 0, s0, 0x2d) == 0 || (int32_t)(base * v) / 100 > 0) {
            apply(1, 0x11, v);
            apply(1, 0x12, v);
            return s2;
        }
        if (e.rd8(t + 0xfe)) apply(0, 0x16, 1);
        if (e.rd8(t + 0x102)) apply(0, 0x18, 1);
        if (e.h(0x24c30, 0, 0, 0, 0, 0, s0)) apply(0, 0xa0, 1);
        return s2;
    }
    for (uint32_t stat = 0x11; stat <= 0x12; ++stat) {
        uint32_t v = e.h(0x463f0, e.rd32(s1 + 0xc), e.rd32(s1 + 8), s0);
        e.h(0x46e50, 0, 0, 0, 0, 0, stat);
        if (!v) continue;
        const uint32_t r = e.h(0x47020, s0, d, 0, 0, 0, s4, s5);
        if (!r) { fatal(0x29d); return 0; }
        if (s3) {
            const uint32_t tb = g_d2common_99e1c;
            uint32_t x = 0;
            if ((int32_t)e.rd32(tb + 0xbd4) > (int32_t)stat) {
                const uint32_t p = e.rd32(tb + 0xbcc) + stat * 0x144;
                if (p) {
                    e.ed(stat << 16);
                    x = e.h(0x38b00, 0, 0, 0, r, 0, p);
                    e.edi(0x38b00, stat << 16);
                }
            }
            v = 0u - x;
        }
        e.h(0x3a330, 0, 0, 0, 0, 0, r, stat, v, 0);
    }
    return s2;
}
}
