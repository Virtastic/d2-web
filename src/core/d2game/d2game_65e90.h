// 1.13c D2Game+0x65E90 (sub_65E90): stack args A0 = pGameCtx, A1 = pRecord (ret 8, return value is leftover EAX and not compared).
// Written from the disassembly as a rule over an environment E (hookenv.h). Returns unless byte [A1+0x118] != 0 and byte [A1+0x119] == 0 and word [A1+0x88] != 0.
// For each of the [A1+0x88] keys at A1+0x68 (4 bytes apart; the key words are read as the loop goes): bucket A0[4] + 0x1520 + 4 * (key & 0x7F), next at +0xE4, match
// [u+0xC] == key (A0[4] == 0 is the assert 0x2D3 path, not captured). On a match, while edi (the running 0xEF650 result) is 0: x = [A1+0x110], y = [A1+0x114];
// 0xA478(u); 0x1280 (EBX = x, EDI = y) -> eax; eax != 0 gives edi = 0xEF650(A0[4], eax, x, y, -1, 8) (EDX 0xC, ECX 0x146); a zero edi falls to 0xA544(u, &xy),
// x += 5, y += 5, 0xA478(u), 0x1280 again, edi = 0xEF650 again (zero skips the unit). Then byte [A1+0x119] = 1; ebx = [[u+0x2C]+0x10] for unit type 2 or 4..5, else
// [u+0x2C] ? 0xA3B8([u+0x2C]) : 0; eax = [[u+0x2C]+0xC] for the same types, else [u+0x2C] ? 0xA454([u+0x2C]) : 0; s = 0x0A0000(A0[4], 0, 1, 0x14C, eax, ebx) with ESI = edi;
// s != 0: 0xA50E(s, key), 0xAF5E(s, 0), 0xA51A(0xA478(s), 0).
#pragma once
#include <cstdint>

namespace d2g65e90 {
template <class E> void Run(E& e, uint32_t a0, uint32_t a1)
{
    auto rd16 = [&](uint32_t a) { return (uint32_t)(e.rd8(a) | e.rd8(a + 1) << 8); };
    if (!e.rd8(a1 + 0x118) || e.rd8(a1 + 0x119) || !rd16(a1 + 0x88)) return;
    uint32_t ptr = a1 + 0x68, edi = 0;
    for (int32_t idx = 0;;) {
        const uint32_t game = e.rd32(a0 + 4), key = e.rd32(ptr);
        if (!game) {   // assert 0x2D3 (0xA2DA, 0xA2C2, 0x22A3C), then int3: not part of the capture
            const uint32_t f = e.h(0xA2DA);
            e.h(0xA2C2, 0, 0, 0, 0, 0, 0xF9FF9, f, 0x2D3);
            e.h(0x22A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
            return;
        }
        uint32_t u = e.rd32(game + (key & 0x7F) * 4 + 0x1520);
        while (u && e.rd32(u + 0xC) != key) u = e.rd32(u + 0xE4);
        if (u) {
            uint32_t xy[2] = { e.rd32(a1 + 0x110), e.rd32(a1 + 0x114) };
            bool skip = false;
            if (!edi) {
                e.h(0xA478, 0, 0, 0, 0, 0, u);
                uint32_t r = e.h(0x1280, 0, 0, 0, xy[0], 0);
                e.edi(0x1280, xy[1]);
                if (r) edi = e.h(0xEF650, 0, 0x146, 0xC, 0, 0, e.rd32(a0 + 4), r, xy[0], xy[1], 0xFFFFFFFFu, 8);
                if (!edi) {
                    e.h(0xA544, 0, 0, 0, 0, 0, u, e.stk(xy));
                    xy[0] += 5; xy[1] += 5;
                    e.h(0xA478, 0, 0, 0, 0, 0, u);
                    r = e.h(0x1280, 0, 0, 0, xy[0], 0);
                    e.edi(0x1280, xy[1]);
                    edi = e.h(0xEF650, 0, 0x146, 0xC, 0, 0, e.rd32(a0 + 4), r, xy[0], xy[1], 0xFFFFFFFFu, 8);
                    if (!edi) skip = true;
                }
            }
            if (!skip) {
                e.wr8(a1 + 0x119, 1);
                const int32_t t = (int32_t)e.rd32(u);
                const uint32_t sub = e.rd32(u + 0x2C);
                const bool direct = t == 2 || (t > 3 && t <= 5);
                const uint32_t ebx = direct ? e.rd32(sub + 0x10) : (sub ? e.h(0xA3B8, 0, 0, 0, 0, 0, sub) : 0);
                const uint32_t eax = direct ? e.rd32(sub + 0xC) : (sub ? e.h(0xA454, 0, 0, 0, 0, 0, sub) : 0);
                const uint32_t s = e.h(0xA0000, 0, 0, 0, 0, edi, e.rd32(a0 + 4), 0, 1, 0x14C, eax, ebx);
                if (s) {
                    e.h(0xA50E, 0, 0, 0, 0, 0, s, key);
                    e.h(0xAF5E, 0, 0, 0, 0, 0, s, 0);
                    e.h(0xA51A, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, s), 0);
                }
            }
        }
        ++idx; ptr += 4;
        if (!(idx < (int32_t)rd16(a1 + 0x88))) break;
    }
}
}  // namespace d2g65e90
