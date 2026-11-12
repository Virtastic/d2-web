// 1.13c Storm+0x21C70 -- cdecl read callback (destination, pointer to count, stream): copies min(count, stream end +0x14 minus stream position +0x10) bytes from the stream buffer (+0xC) at the position, advances the position and returns the byte count.
#include <cstdint>
static inline uint32_t rd32(uint32_t a) { uint32_t v; __builtin_memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr32(uint32_t a, uint32_t v) { __builtin_memcpy((void*)(uintptr_t)a, &v, 4); }
uint32_t storm_21c70(uint32_t data, uint32_t pcount, uint32_t st)
{
    const uint32_t pos = rd32(st + 0x10), left = rd32(st + 0x14) - pos;
    uint32_t n = rd32(pcount);
    if (n >= left) n = left;
    __builtin_memcpy((void*)(uintptr_t)data, (const void*)(uintptr_t)(rd32(st + 0xc) + pos), n);
    wr32(st + 0x10, rd32(st + 0x10) + n);
    return n;
}
