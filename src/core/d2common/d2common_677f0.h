// 1.13c D2Common+0x677F0: path callback for a single step (table entry). ecx = the path request: the start
// point at +0 (two 16-bit words), the wanted point at +4 and the path at +0x30. The path's step count (+0x24) is
// cleared and its first point (+0x9C) set to the wanted point, which is also handed in a stack slot (edi = its
// address) to the adjuster at 0x67630 (eax = the path). When the adjusted point equals the start point the result
// is 0. Otherwise it becomes the path's first point, the point count (+0x28) is 1 and the result is 1.
#pragma once
#include <cstdint>

namespace c677f0 {
template <class E> uint32_t Step(E& e, uint32_t req)
{
    const uint32_t path = e.rd32(req + 0x30);
    e.wr32(path + 0x24, 0);
    uint32_t pt[1] = { e.rd32(req + 4) };
    e.wr32(path + 0x9C, pt[0]);
    e.ed(e.stk(pt));
    e.h(0x67630, path);
    e.edi(0x67630, e.stk(pt));
    if (e.rd32(req) == pt[0]) return 0;
    e.wr32(path + 0x9C, pt[0]);
    e.wr32(path + 0x28, 1);
    return 1;
}
}
