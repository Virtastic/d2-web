// 1.13c D2Client+0x94AA0 -- One stack word: a mouse message (x the word +0xC, y the word +0xE, both zero-extended and compared signed). With the dword 0x11C3A4 set, no player unit at 0x11BBFC, a player whose dword +0x10 is 0x11, or a zero dword 0xE0EA0, the message dword +0x18 is cleared and nothing else happens. Otherwise 0x908C0 runs and the player dword +0x60 is the room. When 0x26320 (EAX = the message) answers zero and 0x8C580 (EAX = x, ECX = y) non-zero, the message dwords +0x18 and +0x1C become 1 and 0 and the point is placed: inside the rectangle of four dwords at 0x10B3E4 (x low, x high, y low, y high, inclusive) D2Common ordinal 11017 (thunk 0xC202, the room) is asked and 0x945D0 runs with EAX = the table 0x10B3E0, EDX = the player and the stack words room, that answer, x, y, 0; the dword 0x11BC68 is cleared and 0xE0EA4 becomes 1. Else the 11 rectangles of 0x14 bytes at 0x11BCA0 are tried in order and the first hit runs 0x8BC30 (EAX = the player, stack: room, index) and sets 0xE0EA4 to 1; with no hit 0x11BC68, the message dword +0x18 and the dwords 0x11BC28, 0x11BC2C, 0x11BC38 and 0x11BC3C are cleared and 0xE0EA4 becomes 1. Otherwise the dword 0x11BC34 decides, the byte 0x11BC48 being saved and restored around each case. 0xB: 0x8C550 (EAX = x, ECX = y) hit sets the byte to 1 and runs the 0x945D0 call with the table 0x10B370 and a last word 1; else a 0x8C520 hit sets 2 and uses 0x10B288 and 2; with neither, the message dword +0x18 and the four dwords are cleared; 0xE0EA4 becomes 1. 0xC: a 0x8C4C0 hit sets 4 and uses the table 0x10B2D0 when the dword 0x119854 is set and 0x10B398 otherwise, last word 4. 0xE: a 0x8C440 hit sets 3 and uses 0x10B3C8 and 3. A miss in 0xC or 0xE clears the message dword and the four dwords; both set 0xE0EA4 to 1. Any other value clears the message dword +0x18 and, only with a zero dword 0xFAE10, the four dwords and 0xE0EA4. The 0x945D0 or 0x8BC30 answer is what the caller sees in EAX on the first-rectangle, table-hit, 0xC-hit and 0xE-hit paths; every other path leaves 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c3a4;
__attribute__((weak)) uint32_t g_d2client_fae10;
__attribute__((weak)) uint32_t g_d2client_119854;
__attribute__((weak)) uint32_t g_d2client_11bbfc;
__attribute__((weak)) uint8_t g_d2client_e0ea0[8];
__attribute__((weak)) uint8_t g_d2client_10b3e4[16];
__attribute__((weak)) uint8_t g_d2client_11bc28[344];
static inline uint32_t gd(const uint8_t* a, uint32_t off) { uint32_t v; __builtin_memcpy(&v, a + off, 4); return v; }
static inline void gw(uint8_t* a, uint32_t off, uint32_t v) { __builtin_memcpy(a + off, &v, 4); }
namespace d2cl {
uint32_t f94aa0(GE& e, uint32_t msg)
{
    uint8_t* const B = g_d2client_11bc28;   // +0 0x11BC28, +4 0x11BC2C, +0xC 0x11BC34, +0x10 0x11BC38, +0x14 0x11BC3C, +0x20 the byte 0x11BC48, +0x40 0x11BC68, +0x78 the 11 rectangles at 0x11BCA0
    uint8_t* const E = g_d2client_e0ea0;    // +0 0xE0EA0, +4 0xE0EA4
    const uint8_t* const Q = g_d2client_10b3e4;
    const uint32_t pl = g_d2client_11bbfc;
    if (g_d2client_11c3a4 || !pl || e.rd32(pl + 0x10) == 0x11 || !gd(E, 0)) { e.wr32(msg + 0x18, 0); return 0; }
    const int32_t x = (int32_t)(e.rd8(msg + 0xc) | e.rd8(msg + 0xd) << 8), y = (int32_t)(e.rd8(msg + 0xe) | e.rd8(msg + 0xf) << 8);
    e.h(0x908C0);
    const uint32_t room = e.rd32(pl + 0x60);
    const auto zero4 = [&]() { gw(B, 0, 0); gw(B, 4, 0); gw(B, 0x10, 0); gw(B, 0x14, 0); };
    const auto in = [&](const uint8_t* q) { return x >= (int32_t)gd(q, 0) && x <= (int32_t)gd(q, 4) && y >= (int32_t)gd(q, 8) && y <= (int32_t)gd(q, 0xc); };
    const auto go = [&](uint32_t tbl, uint32_t k) {
        const uint32_t w = e.h(0xC202, 0, 0, 0, 0, 0, room);
        const uint32_t r = e.h(0x945D0, tbl, 0, pl, 0, 0, room, w, (uint32_t)x, (uint32_t)y);
        const uint32_t t[1] = { k }; e.x(0x945D0, t, 1);
        return r;
    };
    if (!e.h(0x26320, msg) && e.h(0x8C580, (uint32_t)x, (uint32_t)y)) {
        e.wr32(msg + 0x18, 1); e.wr32(msg + 0x1c, 0);
        if (in(Q)) { const uint32_t r = go(0xB210B3E0u, 0); gw(B, 0x40, 0); gw(E, 4, 1); return r; }
        for (uint32_t i = 0; i < 11; ++i)
            if (in(B + 0x78 + 0x14 * i)) { const uint32_t r = e.h(0x8BC30, pl, 0, 0, 0, 0, room, i); gw(E, 4, 1); return r; }
        gw(B, 0x40, 0); e.wr32(msg + 0x18, 0); zero4(); gw(E, 4, 1);
        return 0;
    }
    const uint32_t st = gd(B, 0xc);
    const uint8_t sv = B[0x20];
    if (st == 0xb) {
        bool done = false;
        if (e.h(0x8C550, (uint32_t)x, (uint32_t)y)) { B[0x20] = 1; go(0xB210B370u, 1); done = true; }
        else if (e.h(0x8C520, (uint32_t)x, (uint32_t)y)) { B[0x20] = 2; go(0xB210B288u, 2); done = true; }
        B[0x20] = sv;
        if (!done) { e.wr32(msg + 0x18, 0); zero4(); }
        gw(E, 4, 1);
        return 0;
    }
    if (st == 0xc || st == 0xe) {
        const bool c = st == 0xc;
        if (c ? e.h(0x8C4C0, (uint32_t)x, (uint32_t)y) : e.h(0x8C440, (uint32_t)x, (uint32_t)y)) {
            B[0x20] = c ? 4 : 3;
            // the 0x119854 test sits after the ordinal call in retail; nothing between them can change it
            const uint32_t r = go(c ? (g_d2client_119854 ? 0xB210B2D0u : 0xB210B398u) : 0xB210B3C8u, c ? 4 : 3);
            B[0x20] = sv; gw(E, 4, 1);
            return r;
        }
        e.wr32(msg + 0x18, 0); zero4(); B[0x20] = sv; gw(E, 4, 1);
        return 0;
    }
    e.wr32(msg + 0x18, 0);
    if (!g_d2client_fae10) { zero4(); gw(E, 4, 0); }
    return 0;
}
}
