// 1.13c D2Client+0x4B230 -- ECX = p1, EDX = p2, one stack word p3. 0x45FF0 (BX = the word 0x11966F) gives the word 0x119671. A word 0x11966F of 0x251 runs 0x88A70 with EBX = 0xF1 and four zero stack words; 0x21A the same with EBX = 0xED. With the byte 0x119661 not 1: the byte and the dword 0x11BF94 are cleared, 0x11BF98 takes p1, the word 0x11966F becomes 0xFFFF, 0x48930 runs, 0 is returned. Otherwise, when the word 0x119671 is not 0xFFFF and 0x46150 answers non-zero: the word 0x119673 takes 0x119671, 0x7D3C0 runs (a second 0x46150 answer, the word), the word is appended to the list at 0x119792 when its count (word 0x1197A6) is below 9, the word 0x11966F takes it, 0x11BF94 becomes the address of this function, 0x11BF98 is cleared, the 9-byte packet {0x31, the dword 0x11971D, the word as a dword} goes out through 0x143E0 (EBX = 9), 0x45FF0 (BX = the word) refreshes 0x119671 and 1 is returned. Otherwise the byte 0x119661 and the dwords 0x11BF94 and 0x11BF98 are cleared, 0x46150 is called, 0x45EB0 gives an index; when p2 or p3 is zero, 0x45C50 (EAX = the dword 0x119725) answers a zero low byte, the index is not negative, 0x46150 answers non-zero, the index is below the dword 0xF42F0 and the byte 0xF3BC6 + 0x27 * index is non-zero, the dword 0x119663 becomes 1 and, when 0x46150 answers non-zero again, 0x4A4F0 runs (one more 0x46150 answer, 1) and 1 is returned. Every other case runs 0x48150 and returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11bf94;
__attribute__((weak)) uint32_t g_d2client_11bf98;
__attribute__((weak)) uint32_t g_d2client_f42f0;
__attribute__((weak)) uint8_t g_d2client_119790[28];
__attribute__((weak)) uint8_t g_d2client_f3bc4[256];
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
static inline uint32_t gd(const uint8_t* a, uint32_t off) { uint32_t v; __builtin_memcpy(&v, a + off, 4); return v; }
static inline void gw(uint8_t* a, uint32_t off, uint32_t v) { __builtin_memcpy(a + off, &v, 4); }
namespace d2cl {
uint32_t f4b230(GE& e, uint32_t p1, uint32_t p2, uint32_t ebx0, uint32_t p3)
{
    uint8_t* const L = g_d2client_119790;   // +2 the word list 0x119792, +0x16 its count 0x1197A6
    w16(0x119671, e.h(0x45FF0, 0, 0, 0, (ebx0 & 0xffff0000u) | u16(0x11966f)));
    if (u16(0x11966f) == 0x251) e.h(0x88A70, 0, 0, 0, 0xf1, 0, 0, 0, 0, 0);
    if (u16(0x11966f) == 0x21a) e.h(0x88A70, 0, 0, 0, 0xed, 0, 0, 0, 0, 0);
    if (u8(0x119661) != 1) {
        w8(0x119661, 0);
        g_d2client_11bf94 = 0;
        g_d2client_11bf98 = p1;
        w16(0x11966f, 0xffff);
        e.h(0x48930);
        return 0;
    }
    if (u16(0x119671) != 0xffff && e.h(0x46150)) {
        const uint32_t w = u16(0x119671);
        w16(0x119673, w);
        const uint32_t o = e.h(0x46150);
        e.h(0x7D3C0, 0, 0, 0, 0, 0, o, w);
        const uint32_t n = (uint32_t)L[0x16] | (uint32_t)L[0x17] << 8;
        if (n < 9) {
            L[2 + 2 * n] = (uint8_t)u16(0x119673); L[3 + 2 * n] = (uint8_t)(u16(0x119673) >> 8);
            L[0x16] = (uint8_t)(n + 1); L[0x17] = (uint8_t)((n + 1) >> 8);
        }
        const uint32_t x = u16(0x119673);
        w16(0x11966f, x);
        g_d2client_11bf94 = 0xB204B230u;
        g_d2client_11bf98 = 0;
        uint8_t pk[9]; pk[0] = 0x31;
        { const uint32_t id = u32(0x11971d); __builtin_memcpy(pk + 1, &id, 4); __builtin_memcpy(pk + 5, &x, 4); }
        snd(e, pk, 9);
        w16(0x119671, e.h(0x45FF0, 0, 0, 0, u16(0x119673)));
        return 1;
    }
    w8(0x119661, 0);
    g_d2client_11bf94 = 0;
    g_d2client_11bf98 = 0;
    e.h(0x46150);
    const int32_t idx = (int32_t)e.h(0x45EB0);
    if ((!p2 || !p3) && !(e.h(0x45C50, u32(0x119725)) & 0xff) && idx >= 0 && e.h(0x46150) && idx < (int32_t)g_d2client_f42f0
        && g_d2client_f3bc4[2 + (uint32_t)idx * 0x27]) {
        w32(0x119663, 1);
        if (e.h(0x46150)) {
            const uint32_t o = e.h(0x46150);
            e.h(0x4A4F0, 0, 0, 0, 0, 0, o, 1);
            return 1;
        }
    }
    e.h(0x48150);
    return 0;
}
}
