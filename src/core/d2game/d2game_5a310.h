// 1.13c D2Game+0x5A310 (ACT5Q4 unit-event callback): ecx = quest node, edx = callback args, plain ret, no result.
// Written from the 1.13c disassembly as a rule over an environment E (HookEnv shape); E::addr(p) is a local's address as the callee sees it.
// q = node+0x18 record; node byte +9 clear returns. unit = args+8.
// unit of class 0x1B2 (+4): import 0xA478(unit) (ret 4) goes to import 0xA51A(room, 0) (ret 8); room = 0xA478(args+8) again; nonzero:
// import 0xA55C(room, &list, &count) (ret 0xC) and, for every listed room's unit chain (+0x74, next +0xE8), a unit of type 1, class 0x216,
// mode (+0x10) neither 0xC nor 0, with import 0xAB86(u, args+8) (ret 8) below 0xF signed, appends u+0xC to q+0x120[q+0x15C], bumps
// q+0x15C and q+0xAC. Any other unit: its id (+0xC, -1 without one) found in q+0x120[0 .. q+0x15C) -> k = 0x58260 (eax = unit, ecx = node;
// plain ret); k < 0 returns; q+0x114[k] += 1, reaching 5 sets byte q+0x108+k; return. Not found: q+0xA8 += 1.
// Tail: q+0xA8 >= 5 and node byte +0xB != 0xC: +0x14 := 0, +0xB := 0xC, 0xDFCC0 (eax 0, ebx = module 0x593F0, edi = node+4, stack 0);
// q+0x84 := 0; 0xDFCC0 (ebx = module 0x59390, edi = node+4, stack args+0xC). Then k = 0x58260(args+8, node): k >= 0 -> the
// q+0x114[k] / q+0x108+k step (q re-read from node+0x18).
#pragma once
#include <cstdint>

namespace d2g5a310 {
template <class E> void bump(E& e, uint32_t q, int32_t k)
{
    const uint32_t v = e.rd32(q + 0x114 + 4 * k) + 1;
    e.wr32(q + 0x114 + 4 * k, v);
    if ((int32_t)v >= 5) e.wr8(q + 0x108 + k, 1);
}
template <class E> void UnitEvent(E& e, uint32_t node, uint32_t msg)
{
    const uint32_t q = e.rd32(node + 0x18);
    if (!e.rd8(node + 9)) return;
    const uint32_t unit = e.rd32(msg + 8);
    if (unit && e.rd32(unit + 4) == 0x1B2) {
        const uint32_t r0 = e.h(0xA478, 0, 0, 0, 0, 0, unit);
        e.h(0xA51A, 0, 0, 0, 0, 0, r0, 0);
        const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, e.rd32(msg + 8));
        if (room) {
            uint32_t list = 0, count = 0;
            e.h(0xA55C, 0, 0, 0, 0, 0, room, e.addr(&list), e.addr(&count));
            for (uint32_t i = 0; i < count; ++i)
                for (uint32_t u = e.rd32(e.rd32(list + 4 * i) + 0x74); u; u = e.rd32(u + 0xE8)) {
                    if (e.rd32(u) != 1 || e.rd32(u + 4) != 0x216) continue;
                    const uint32_t m = e.rd32(u + 0x10);
                    if (m == 0xC || m == 0) continue;
                    if ((int32_t)e.h(0xAB86, 0, 0, 0, 0, 0, u, e.rd32(msg + 8)) >= 0xF) continue;
                    e.wr32(q + 0x120 + 4 * e.rd32(q + 0x15C), e.rd32(u + 0xC));
                    e.wr32(q + 0x15C, e.rd32(q + 0x15C) + 1);
                    e.wr32(q + 0xAC, e.rd32(q + 0xAC) + 1);
                }
        }
    } else {
        for (int32_t i = 0; i < (int32_t)e.rd32(q + 0x15C); ++i) {
            const uint32_t id = unit ? e.rd32(unit + 0xC) : 0xFFFFFFFFu;
            if (id == e.rd32(q + 0x120 + 4 * i)) {
                const int32_t k = (int32_t)e.h(0x58260, unit, node);
                if (k >= 0) bump(e, q, k);
                return;
            }
        }
        e.wr32(q + 0xA8, e.rd32(q + 0xA8) + 1);
    }
    if ((int32_t)e.rd32(q + 0xA8) >= 5 && e.rd8(node + 0xB) != 0xC) {
        e.wr8(node + 0x14, 0);
        e.wr8(node + 0xB, 0xC);
        uint32_t g = e.rd32(node + 4);
        e.h(0xDFCC0, 0, 0, 0, 0xB20593F0u, 0, 0);
        e.edi(0xDFCC0, g);
        e.wr8(q + 0x84, 0);
        const uint32_t t = e.rd32(msg + 0xC);
        g = e.rd32(node + 4);
        e.h(0xDFCC0, 0, 0, 0, 0xB2059390u, 0, t);
        e.edi(0xDFCC0, g);
    }
    const int32_t k = (int32_t)e.h(0x58260, e.rd32(msg + 8), node);
    if (k >= 0) bump(e, e.rd32(node + 0x18), k);
}
}
