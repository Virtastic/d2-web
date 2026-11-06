// 1.13c D2Game+0x6A7A0 (sub_6A7A0): ECX = ctx {game, a, b}, plain ret, returns 1.
// Written from the disassembly as a rule over an environment E (hookenv.h). edi = [b+0x6C] ? 0xE03A0(EAX [b+0x68], ECX game, EDX [b+0x64]) : 0;
// ebp = ([b] == 0) ? [0xA4C0(b) + 0x9C] : 0.
//   a != 0 and [a+0x10] != 0: when [a+0x10] == 3 and edi == a: [b+0x64] = -1, [b+0x68] = 6, byte [b+0x6C] = 0, 0xA4A2(a, 1); return 1.
//   else (a null or [a+0x10] == 0): when [b] == 0 the call returns 1 on [b+0x6C] != 0, on 0xA6A6([b+0x60]) != 0 and on [0xA4C0(b) + 0x4C] == 1;
//   0x698E0(EAX = b, EDI = 0x14) == 0 calls 0xDFFD0(0x13, b); otherwise 0xDFAE0(EAX = b, 2, a ? [a+0xC] : -1), 0xA4A2(a, 3) and
//   0x8AB20(EAX = &{0x58, a ? [a+0xC] : -1, 0}, ebp).
#pragma once
#include <cstdint>
#include <cstring>

namespace d2g6a7a0 {
template <class E> uint32_t Run(E& e, uint32_t ctx)
{
    const uint32_t a = e.rd32(ctx + 4), b = e.rd32(ctx + 8);
    uint32_t edi = 0;
    if (e.rd8(b + 0x6C)) edi = e.h(0xE03A0, e.rd32(b + 0x68), e.rd32(ctx), e.rd32(b + 0x64));
    uint32_t ebp = 0;
    if (b && e.rd32(b) == 0) ebp = e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, b) + 0x9C);
    if (a && e.rd32(a + 0x10)) {
        if (e.rd32(a + 0x10) == 3 && edi == a) {
            e.wr32(b + 0x64, 0xFFFFFFFFu); e.wr32(b + 0x68, 6); e.wr8(b + 0x6C, 0);
            e.h(0xA4A2, 0, 0, 0, 0, 0, a, 1);
        }
        return 1;
    }
    if (b && e.rd32(b) == 0) {
        if (e.rd8(b + 0x6C)) return 1;
        if (e.h(0xA6A6, 0, 0, 0, 0, 0, e.rd32(b + 0x60))) return 1;
        if (e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, b) + 0x4C) == 1) return 1;
    }
    const uint32_t r = e.h(0x698E0, b);
    e.edi(0x698E0, 0x14);
    if (!r) { e.h(0xDFFD0, 0, 0, 0, 0, 0, 0x13, b); return 1; }
    const uint32_t id = a ? e.rd32(a + 0xC) : 0xFFFFFFFFu;
    e.h(0xDFAE0, b, 0, 0, 0, 0, 2, id);
    e.h(0xA4A2, 0, 0, 0, 0, 0, a, 3);
    uint8_t buf[8] = {0x58};
    memcpy(buf + 1, &id, 4);
    uint32_t w[2]; memcpy(w, buf, 8);
    e.h(0x8AB20, e.stk(buf), 0, 0, 0, 0, ebp);
    e.blk(0x8AB20, w, 2, 3);
    return 1;
}
}  // namespace d2g6a7a0
