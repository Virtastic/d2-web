// 1.13c D2Common+0x677C0: path-type handler "forced move" (table entry). ecx = the path request: +0x04 the target
// point word, +0x30 the dynamic path. The path's point count (+0x24) is cleared, its first point slot (+0x9C) takes
// the target word, and the collision walk (0x67630: eax = path, edi = the point slot) runs. Result 1.
#pragma once
#include <cstdint>

namespace c677c0 {
template <class E> uint32_t ForceMove(E& e, uint32_t req)
{
    const uint32_t path = e.rd32(req + 0x30);
    e.wr32(path + 0x24, 0);
    const uint32_t target = e.rd32(req + 4);
    e.wr32(path + 0x9C, target);
    e.h(0x67630, path, target);
    e.j(0x67630, path + 0x9C);
    return 1;
}
}
