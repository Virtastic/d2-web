// 1.13c Storm+0x21C30 -- cdecl write callback (source, pointer to count, stream): copies min(count, stream end +8 minus stream position +4) bytes to the stream buffer (+0) at the position, advances the position and returns the stream pointer.
#include <cstdint>
static inline uint32_t rd32(uint32_t a) { uint32_t v; __builtin_memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr32(uint32_t a, uint32_t v) { __builtin_memcpy((void*)(uintptr_t)a, &v, 4); }
uint32_t storm_21c30(uint32_t data, uint32_t pcount, uint32_t st)
{
    const uint32_t pos = rd32(st + 4), room = rd32(st + 8) - pos;
    uint32_t n = rd32(pcount);
    if (n >= room) n = room;
    __builtin_memcpy((void*)(uintptr_t)(rd32(st) + pos), (const void*)(uintptr_t)data, n);
    wr32(st + 4, rd32(st + 4) + n);
    return st;
}
