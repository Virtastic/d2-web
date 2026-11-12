// 1.13c D2Client+0x46AF0 -- No arguments. A set dword 0x119763 runs 0xA3D20 and is cleared. A box already at 0x11975F halts retail. Asks 0x46880 for a position pair (ESI = the zeroed pair), opens the box with 0xA3E40 (pair, the callback 0x45BF0, 0, 0, 0, 1, 0, 0, 0, 1), stores it at 0x11975F, sets 0x11974B to GetTickCount + 60000 and clears 0x11974F. A null box halts retail. The box dword +0x5C is cleared, the string 0xD19 is fetched (0xD1EC, ECX = the index) and set on the box with 0xA36F0 (ECX = the box, EAX = 0, stack: string, 0xF, 0, 1, 0, 0). Then the dword 0x119663 becomes 10, 0x119729 becomes 4, 0x11978E is cleared and the 5-byte packet {0x34, the dword 0x11971D} goes out through 0x143E0 (stack buffer, EBX = 5). Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11978e;
__attribute__((weak)) uint8_t g_d2client_119661[0x118];   // the packed UI block 0x119661..0x119779
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
uint32_t f46af0(GE& e, uint32_t tick)
{
    if (u32(0x119763)) { e.h(0xA3D20); w32(0x119763, 0); }
    if (u32(0x11975f)) return 0;   // retail halts here
    uint32_t pr[2] = { 0, 0 };
    e.h(0x46880, 0, 0, 0, 0, e.stk(pr));
    const uint32_t box = e.h(0xA3E40, 0, 0, 0, 0, 0, pr[0], pr[1], 0xB2045BF0u, 0);
    { const uint32_t w[7] = { 0, 0, 1, 0, 0, 0, 1 }; e.x(0xA3E40, w, 7); }
    w32(0x11975f, box);
    w32(0x11974b, tick + 60000);
    w32(0x11974f, 0);
    if (!box) return 0;   // retail halts here
    e.wr32(box + 0x5c, 0);
    const uint32_t str = e.h(0xD1EC, 0, 0xd19);
    e.h(0xA36F0, 0, box, 0, 0, 0, str, 0xf, 0, 1);
    { const uint32_t w[2] = { 0, 0 }; e.x(0xA36F0, w, 2); }
    w32(0x119663, 10);
    w32(0x119729, 4);
    g_d2client_11978e = 0;
    uint8_t pk[5]; pk[0] = 0x34; { const uint32_t id = u32(0x11971d); __builtin_memcpy(pk + 1, &id, 4); }
    snd(e, pk, 5);
    return 1;
}
}
