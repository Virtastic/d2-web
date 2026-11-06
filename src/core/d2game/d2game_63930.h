// 1.13c D2Game+0x63930 (sub_63930): ECX = pCtx, EDX = pUnit, [esp+4] = unused word (ret 4), returns 0.
// Written from the disassembly as a rule over an environment E (hookenv.h). ebx = 0xA478(unit). The node list at [[ctx+0x10F4]] (next at +0xF4) is searched for
// type 2; a miss logs the assert (0xA2DA file, 0xA2C2(str, file, 0x133)) and 0x22A3C(-1), whose result stands in as the node. dat = [node+0x18]; byte
// [dat+2] == 0 returns. The bucket ctx+0x1320+4*([dat+8] & 0x7F) (next at +0xE4) is searched for [b+0xC] == [dat+8]; esi = 0xA478(bucket node or 0);
// ebx == 0 returns. edi = 1 when ebx == esi, else g = 0xA478(unit) (assert path at 0x14E), 0xA55C(g, &arr, &count) and edi = 1 when arr[i] == esi.
// p = 0xA4C0(unit); s = [p + 4*byte[ctx+0x6D] + 0x10]; 0xA568(s, 2, 0) or 0xA568(s, 2, 1) nonzero returns; edi != 0 calls 0xA57A(s, 2, 0xD) and 0xA57A(s, 2, 1).
#pragma once
#include <cstdint>

namespace d2g63930 {
template <class E> uint32_t Run(E& e, uint32_t ctx, uint32_t unit)
{
    const uint32_t ebx = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    uint32_t node = 0;
    bool found = false;
    uint32_t head = e.rd32(ctx + 0x10F4);
    if (head) for (node = e.rd32(head); node; node = e.rd32(node + 0xF4)) if (e.rd32(node) == 2) { found = true; break; }
    if (!found) {   // assert 0x133 (string 0x6FD19FF9 in retail; this path is not part of the capture), then 0x22A3C(-1) returns and the code goes on with its result
        const uint32_t f = e.h(0xA2DA);
        e.h(0xA2C2, 0, 0, 0, 0, 0, 0xF9FF9, f, 0x133);
        node = e.h(0x22A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
    }
    const uint32_t dat = e.rd32(node + 0x18);
    if (!e.rd8(dat + 2)) return 0;
    const uint32_t key = e.rd32(dat + 8);
    uint32_t b = e.rd32(ctx + (key & 0x7F) * 4 + 0x1320);
    while (b && e.rd32(b + 0xC) != key) b = e.rd32(b + 0xE4);
    const uint32_t esi = e.h(0xA478, 0, 0, 0, 0, 0, b);
    if (!ebx) return 0;
    uint32_t edi = 0;
    if (ebx == esi) edi = 1;
    else {
        uint32_t g = e.h(0xA478, 0, 0, 0, 0, 0, unit);
        if (!g) {
            const uint32_t f = e.h(0xA2DA);
            e.h(0xA2C2, 0, 0, 0, 0, 0, 0xF9FF9, f, 0x14E);
            g = e.h(0x22A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
        }
        uint32_t arr = 0, cnt = 0;
        e.h(0xA55C, 0, 0, 0, 0, 0, g, e.stk(&arr), e.stk(&cnt));
        for (uint32_t i = 0; i < cnt; i++) if (e.rd32(arr + 4 * i) == esi) { edi = 1; break; }
    }
    const uint32_t p = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t s = e.rd32(p + e.rd8(ctx + 0x6D) * 4 + 0x10);
    if (e.h(0xA568, 0, 0, 0, 0, 0, s, 2, 0)) return 0;
    if (e.h(0xA568, 0, 0, 0, 0, 0, s, 2, 1)) return 0;
    if (!edi) return 0;
    e.h(0xA57A, 0, 0, 0, 0, 0, s, 2, 0xD);
    e.h(0xA57A, 0, 0, 0, 0, 0, s, 2, 1);
    return 0;
}
}  // namespace d2g63930
