// 1.13c D2Common+0x7C470: level callback (table entry). ecx = the room record. The flags at +0x28 are returned
// as they are unless bit 24 is set, the record is not of kind 2 (+0x48) without bit 25, and the byte at +0x44 is
// above 2; then the routine at 0x7C3D0 takes over (a tail jump with eax = the flags, ecx = the record, edx = 2).
#pragma once
#include <cstdint>

namespace c7c470 {
template <class E> uint32_t Step(E& e, uint32_t rec)
{
    const uint32_t flags = e.rd32(rec + 0x28);
    if (!(flags & 0x1000000)) return flags;
    if (e.rd32(rec + 0x48) == 2 && !(flags & 0x2000000)) return flags;
    if (e.rd8(rec + 0x44) <= 2) return flags;
    return e.h(0x7C3D0, flags, rec, 2);
}
}
