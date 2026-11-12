// 1.13c D2Common+0x67B70: path-type handler "toward, from the start" (table entry). ecx = the path request; the
// dynamic path (+0x30) has its point count (+0x24) cleared and the toward walk (0x678B0: eax = path, ecx = request)
// takes over; its result is the result.
#pragma once
#include <cstdint>

namespace c67b70 {
template <class E> uint32_t TowardFromStart(E& e, uint32_t req)
{
    const uint32_t path = e.rd32(req + 0x30);
    e.wr32(path + 0x24, 0);
    return e.h(0x678B0, path, req);
}
}
