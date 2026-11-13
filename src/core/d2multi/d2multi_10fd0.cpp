// 1.13c D2Multi+0x10FD0 -- stdcall message handler, one stack word = the message (+0 id, +4 code). The id is looked up in the 11-entry table at 0x38E50; not found returns 0. With idx the position and base = [0x39F04], edi = base + idx:
// code 0x205 -- the idx-th node of the list at 0x39EF4 (next at +0xB0; fewer nodes returns 1): worker 0x11510, then the text after the first '*' of the node name (+4; the name itself when there is none or nothing follows)
// is compared with the string at [0x39F14] + 0x89: equal calls 0x13750 (node+0x36, [+0xA0], [+0xA8]), different calls 0x13540 (EAX = name, EDI = node+0x36, stack [+0xA0], [+0xA8]).
// other codes -- GetTickCount; an edi equal to [0x19BC0] only calls 0xE860; an edi at or above the signed count [0x39F00] returns 1; else 0xE860 and the walk: at the idx-th node the handle (+0x9C) goes through 0x7D04 (bit mask),
// is OR-ed with gdwBitMasks[10] (the import at 0x16254, +0x28) and set by 0x7CFE; the node, edi and the tick are stored at 0x39F08, 0x19BC0, 0x39F0C and 0xE760 runs. A null handle reports through 0x7BDE/0x7BD2 and exits(-1) (0x14BA). Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_39f04, g_d2multi_39ef4, g_d2multi_39f14, g_d2multi_39f00, g_d2multi_39f08, g_d2multi_39f0c, g_d2multi_19bc0, g_d2multi_16254, g_d2multi_38e50[11];
namespace d2multi {
uint32_t f10fd0(GE& e, uint32_t msg)
{
    const uint32_t id = e.rd32(msg);
    uint32_t idx = 0;
    while (idx < 11 && g_d2multi_38e50[idx] != id) idx++;
    if (idx == 11) return 0;
    const uint32_t edi = g_d2multi_39f04 + idx;
    if (e.rd32(msg + 4) == 0x205) {
        uint32_t p = g_d2multi_39ef4, k = 0;
        if (!p) return 1;
        while (k != edi) { p = e.rd32(p + 0xb0); k++; if (!p) return 1; }
        e.h(0x11510);
        const uint32_t name = p + 4;
        const uint32_t star = e.h(0x7ba8, 0, 0, 0, 0, 0, name, 0x2a);
        uint32_t a = (star && e.rd8(star + 1)) ? star + 1 : name;
        uint32_t b = g_d2multi_39f14 + 0x89;
        int32_t cmp = 0;
        for (;;) {
            const uint32_t x = e.rd8(a), y = e.rd8(b);
            if (x != y) { cmp = x < y ? -1 : 1; break; }
            if (!x) break;
            a++; b++;
        }
        if (!cmp) e.h(0x13750, 0, 0, 0, 0, 0, p + 0x36, e.rd32(p + 0xa0), e.rd32(p + 0xa8));
        else { e.h(0x13540, name, 0, 0, 0, 0, e.rd32(p + 0xa0), e.rd32(p + 0xa8)); e.j(0x13540, p + 0x36); }
        return 1;
    }
    const uint32_t tick = e.h(0x16370, 0, 0, 0, 0, 0);
    if (edi == g_d2multi_19bc0) { e.h(0xe860); return 1; }
    if ((int32_t)edi >= (int32_t)g_d2multi_39f00) return 1;
    e.h(0xe860);
    uint32_t n = g_d2multi_39ef4, k = 0;
    if (!n) return 1;
    for (;;) {
        if (k == edi) {
            const uint32_t hd = e.rd32(n + 0x9c);
            if (!hd) { const uint32_t t = e.h(0x7bde, 0, 0, 0, 0, 0, 0x12e); e.h(0x7bd2, 0, 0, 0, 0, 0, 0xB20175d7u, t); e.h(0x14ba, 0, 0, 0, 0, 0, 0xffffffffu); return 1; }
            const uint32_t m = e.h(0x7d04, 0, 0, 0, 0, 0, hd);
            e.h(0x7cfe, 0, 0, 0, 0, 0, e.rd32(n + 0x9c), e.rd32(g_d2multi_16254 + 0x28) | m);
            g_d2multi_39f08 = n; g_d2multi_19bc0 = edi; g_d2multi_39f0c = tick;
            e.h(0xe760);
        }
        n = e.rd32(n + 0xb0); k++;
        if (!n) break;
    }
    return 1;
}
}
