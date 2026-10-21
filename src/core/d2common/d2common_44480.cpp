// 1.13c D2Common+0x44480: allocate a zeroed 0x20-byte node (DrlgRoom.cpp, line 0x3b6 stored at +0x14),
// fill +0 = reg arg (decompiler shows it as the return-address slot), +8 = a, +0x18 = b, and push it on
// the list headed at room+0x5c (ESI); with no room the node is returned unlinked (+0xC = 0).
#include <cstdint>
typedef uintptr_t P;
static inline void wr32(P b, uint32_t o, uint32_t v) { *(uint32_t*)(b + o) = v; }

void D2Common_44480(P (*alloc10045)(const char*, int, int), P room, uint32_t first, uint32_t a, uint32_t b)
{
    P n = alloc10045("..\\Source\\D2Common\\DRLG\\DrlgRoom.cpp", 0x3b6, 0);
    for (uint32_t o = 0; o < 0x20; o += 4) wr32(n, o, 0);
    wr32(n, 0x14, 0x3b6);
    wr32(n, 0, first);
    wr32(n, 8, a);
    wr32(n, 0x18, b);
    if (room) { wr32(n, 0xc, *(uint32_t*)(room + 0x5c)); wr32(room, 0x5c, (uint32_t)n); }
}
