// 1.13c D2Client+0x4ABE0 -- No arguments. A set dword 0x119763 runs 0xA3D20 and is cleared. The unit whose id (+0xC) is the dword 0x11971D is looked up in the 128-bucket table at 0x10A808 (bucket = id & 0x7F, next at +0xE4). A missing unit, or one whose dword +0x60 is zero, runs 0x47B20 (EDI = the id) and returns 1; a unit whose type (+0) is not 1 halts retail. Otherwise the dword 0xFADB0 is remembered, 0x119663 becomes 3 and 0x1197A8 becomes 1 (Fog ordinal 10227 is asked; the dword 0xDD5D0 must be 0x26). With a zero dword 0x113A68 the 0x26 panel flags at 0xFAD80 are walked: for a flag equal to 1 the dword +0x30 of the panel record (pointer table 0xDD538) decides: 0 nothing, 1 runs 0xC2790 (ECX = the index, EDX = 1, stack 0), 4 runs 0x47B20 (EDI = the id) when the dword 0x119721 is set, anything else leaves for the packet. After the walk, with no player unit at 0x11BBFC, or a player whose dword +0 is zero and for whom 0xA5440 (EAX = the player) answers zero, 0xFADB0 becomes 1. Remembered zero and now set: 0x11C434 becomes 2, the music block runs (below), then 0x911A0, 0xBFA10 (EDI = the string at 0xD05E0) and 0x3CCC0 (EAX = 8; D2Gfx ordinal 10048, the string at 0xDBC60). Remembered set and now zero: 0x3CC60 (EAX = 8; ordinal 10048, 0xDBC60), 0x492F0, 0x11C434 becomes 1 and the music block runs. The music block, only with a zero dword 0x12349C: that dword becomes 1; a set dword 0x123498 runs 0x3CC60 (EAX = 2; ordinal 10048, the string 0xF51EC) and is cleared; then 0x3CCC0 (EAX = 4; ordinal 10048, the string 0xF51BC). Then 0x11C418 is cleared, 0x11C414 becomes 3 when 0xFADB0 is now set and 0 otherwise, D2Win thunk 0xD1B0 runs with ECX = 0 and 0x8A4D0 with EAX = the dword 0xF9E18, ECX = 0xF9E14, EDX = 0, ESI = 0x11C3C4 and one zero stack word; in the set case the dwords 0x11C3DC, 0x11C3E0, 0x11C3F8 and 0x11C3FC are cleared. Last the 13-byte packet {0x38, dword 2, the id, dword 0} goes out through 0x143E0 (EBX = 0xD), the word 0x11C33C is cleared and the dword 0x11C338 takes the player unit. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_113a68;
__attribute__((weak)) uint32_t g_d2client_11bbfc;
__attribute__((weak)) uint32_t g_d2client_1197a8;
__attribute__((weak)) uint8_t g_d2client_10a808[512];
__attribute__((weak)) uint8_t g_d2client_fad80[152];
__attribute__((weak)) uint8_t g_d2client_dd538[156];
__attribute__((weak)) uint8_t g_d2client_11c338[256];
__attribute__((weak)) uint8_t g_d2client_f9e14[8];
__attribute__((weak)) uint8_t g_d2client_123498[8];
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
uint32_t f4abe0(GE& e)
{
    uint8_t* const FL = g_d2client_fad80;      // [12] = 0xFADB0
    uint8_t* const C = g_d2client_11c338;       // +0 0x11C338, +4 0x11C33C, +0x8C 0x11C3C4, +0xA4 0x11C3DC, +0xA8 0x11C3E0, +0xC0 0x11C3F8, +0xC4 0x11C3FC, +0xDC 0x11C414, +0xE0 0x11C418, +0xFC 0x11C434
    uint8_t* const M = g_d2client_123498;       // +0 0x123498, +4 0x12349C
    if (u32(0x119763)) { e.h(0xA3D20); w32(0x119763, 0); }
    const uint32_t guid = u32(0x11971d);
    uint32_t un = gd(g_d2client_10a808, 4 * (guid & 0x7f));
    while (un && e.rd32(un + 0xc) != guid) un = e.rd32(un + 0xe4);
    if (!un) { e.h(0x47B20); e.edi(0x47B20, guid); return 1; }
    if (e.rd32(un) != 1) return 0;   // retail halts here
    if (!e.rd32(un + 0x60)) { e.h(0x47B20); e.edi(0x47B20, guid); return 1; }
    const uint32_t old = gd(FL, 0x30);
    w32(0x119663, 3);
    g_d2client_1197a8 = 1;
    e.h(0xBF8C);
    bool tail = g_d2client_113a68 != 0;
    if (!tail) {
        uint32_t i = 0;
        do {
            if (gd(FL, 4 * i) == 1) {
                const uint32_t t = e.rd32(gd(g_d2client_dd538, 4 * i) + 0x30);
                if (t == 1) e.h(0xC2790, 0, i, 1, 0, 0, 0);
                else if (t == 4) { if (u32(0x119721)) { e.h(0x47B20); e.edi(0x47B20, u32(0x11971d)); } }
                else if (t != 0) { tail = true; break; }
            }
        } while ((int32_t)++i < (int32_t)gd(g_d2client_dd538, 0x98));
    }
    if (!tail) {
        const uint32_t pl = g_d2client_11bbfc;
        if (!pl || (!e.rd32(pl) && !e.h(0xA5440, pl))) gw(FL, 0x30, 1);
        const uint32_t now = gd(FL, 0x30);
        const auto music = [&]() {
            if (gd(M, 4)) return;
            gw(M, 4, 1);
            if (gd(M, 0)) { const uint32_t w = e.h(0xD0B4); e.h(0x3CC60, 2, 0, 0, 0, 0, w, 0xB20F51ECu); gw(M, 0, 0); }
            const uint32_t w = e.h(0xD0B4);
            e.h(0x3CCC0, 4, 0, 0, 0, 0, w, 0xB20F51BCu);
        };
        if (!old) {
            if (now) {
                gw(C, 0xfc, 2);
                music();
                e.h(0x911A0);
                e.h(0xBFA10); e.edi(0xBFA10, 0xB20D05E0u);
                const uint32_t w = e.h(0xD0B4);
                e.h(0x3CCC0, 8, 0, 0, 0, 0, w, 0xB20DBC60u);
            }
        } else if (!now) {
            const uint32_t w = e.h(0xD0B4);
            e.h(0x3CC60, 8, 0, 0, 0, 0, w, 0xB20DBC60u);
            e.h(0x492F0);
            gw(C, 0xfc, 1);
            music();
        }
        gw(C, 0xe0, 0);
        gw(C, 0xdc, now ? 3 : 0);
        e.h(0xD1B0, 0, 0);
        e.h(0x8A4D0, gd(g_d2client_f9e14, 4), gd(g_d2client_f9e14, 0), 0, 0, gd(C, 0x8c), 0);
        if (now) { gw(C, 0xa4, 0); gw(C, 0xa8, 0); gw(C, 0xc0, 0); gw(C, 0xc4, 0); }
    }
    uint8_t pk[13]; pk[0] = 0x38;
    { const uint32_t a = 2, b = u32(0x11971d), c = 0; __builtin_memcpy(pk + 1, &a, 4); __builtin_memcpy(pk + 5, &b, 4); __builtin_memcpy(pk + 9, &c, 4); }
    snd(e, pk, 13);
    C[4] = 0; C[5] = 0;
    gw(C, 0, g_d2client_11bbfc);
    return 1;
}
}
