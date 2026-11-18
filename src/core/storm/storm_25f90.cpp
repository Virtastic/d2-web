// 1.13c Storm+0x25F90 -- cdecl write callback (source, pointer to count, stream): copies count bytes to the stream buffer (+8) at the position (+0xC), advances the position by the count and returns the stream pointer. No bound is applied.
#include <cstdint>
static inline uint32_t rd32(uint32_t a) { uint32_t v; __builtin_memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wr32(uint32_t a, uint32_t v) { __builtin_memcpy((void*)(uintptr_t)a, &v, 4); }
uint32_t storm_25f90(uint32_t data, uint32_t pcount, uint32_t st)
{
    const uint32_t n = rd32(pcount);
    __builtin_memcpy((void*)(uintptr_t)(rd32(st + 8) + rd32(st + 0xc)), (const void*)(uintptr_t)data, n);
    wr32(st + 0xc, rd32(st + 0xc) + rd32(pcount));
    return st;
}
