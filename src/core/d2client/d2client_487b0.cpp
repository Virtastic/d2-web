// 1.13c D2Client+0x487B0 -- No arguments; returns 1. A set dword 0x119763 runs 0xA3D20 and is cleared. 0x48150 runs, then the 13-byte packet 0x38 is sent (dword 0, the dword 0x11971D, dword 0x28).
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint8_t g_d2client_119661[0x118];   // the packed UI block 0x119660..0x119778
static inline uint32_t u32(uint32_t rva) { uint32_t v; __builtin_memcpy(&v, g_d2client_119661 + (rva - 0x119661), 4); return v; }
static inline void w32(uint32_t rva, uint32_t v) { __builtin_memcpy(g_d2client_119661 + (rva - 0x119661), &v, 4); }
static inline uint32_t u16(uint32_t rva) { uint16_t v; __builtin_memcpy(&v, g_d2client_119661 + (rva - 0x119661), 2); return v; }
static inline void w16(uint32_t rva, uint32_t v) { const uint16_t x = (uint16_t)v; __builtin_memcpy(g_d2client_119661 + (rva - 0x119661), &x, 2); }
static inline uint32_t u8(uint32_t rva) { return g_d2client_119661[rva - 0x119661]; }
static inline void w8(uint32_t rva, uint32_t v) { g_d2client_119661[rva - 0x119661] = (uint8_t)v; }
__attribute__((weak)) uint8_t* g_d2client_wl;   // the send log: dword count, then per packet a length dword and the bytes padded to 4
static inline void snd(GE& e, const uint8_t* b, uint32_t n)
{
    e.h(0x143E0, 0, 0, 0, n, 0, e.stk(b));
    uint32_t cnt; __builtin_memcpy(&cnt, g_d2client_wl, 4);
    uint8_t* p = g_d2client_wl + 4;
    for (uint32_t k = 0; k < cnt; ++k) { uint32_t l; __builtin_memcpy(&l, p, 4); p += 4 + ((l + 3) & ~3u); }
    __builtin_memcpy(p, &n, 4); __builtin_memset(p + 4, 0, (n + 3) & ~3u); __builtin_memcpy(p + 4, b, n);
    ++cnt; __builtin_memcpy(g_d2client_wl, &cnt, 4);
}
namespace d2cl {
uint32_t f487b0(GE& e)
{
    if (u32(0x119763)) {
        e.h(0xA3D20);
        w32(0x119763, 0);
    }
    e.h(0x48150);
    uint8_t b[13];
    const uint32_t z = 0, a = u32(0x11971d), k = 0x28;
    b[0] = 0x38;
    __builtin_memcpy(b + 1, &z, 4);
    __builtin_memcpy(b + 5, &a, 4);
    __builtin_memcpy(b + 9, &k, 4);
    snd(e, b, 13);
    return 1;
}
}
