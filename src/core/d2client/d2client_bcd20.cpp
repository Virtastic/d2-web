// 1.13c D2Client+0xBCD20 -- stdcall message handler (one argument, the message; x,y are the words at +0xC and +0xE). Panel origin X0 = 0x11B9A0, Y0 = 0x11B9A4, W = 0xDBC48, H = 0xDBC4C.
// Inside the strip x in [X0+0x80, X0+0xA0], y in [Y0+H-0x5C, Y0+H-0x3C]: 0x11BBC4 := 1, the message is marked handled, 0x88A70(0,0,0,0; EBX = 4) then Storm 0xBE84(msg).
// Inside the panel (X0 <= x <= W/2-1, Y0 <= y <= Y0+H-0x31) with a nonzero 0xC250(0x11BBFC, 4, 0): the four 14-byte rows at 0xDD810 (x offset, y offset, flag +8) are tested for a 0x28-wide hit; the hit row's flag := 1, 0x16360(), the message is handled, 0x88A70(...; EBX = 5), 0xBE84(msg). No hit: handled and 0xBE84(msg). Elsewhere: 0xBCB30(); a zero result also zeroes the message +0x18; its result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11b9a0, g_d2client_11b9a4, g_d2client_dbc4c, g_d2client_dbc48, g_d2client_11bbc4, g_d2client_11bbfc;
__attribute__((weak)) uint8_t g_d2client_dd810[0x38];
static inline uint32_t b32(const uint8_t* p) { uint32_t v; __builtin_memcpy(&v, p, 4); return v; }
static inline uint32_t u16w(uint32_t a) { uint16_t v; __builtin_memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
namespace d2cl {
uint32_t fbcd20(GE& e, uint32_t msg)
{
    const int32_t X0 = (int32_t)g_d2client_11b9a0, Y0 = (int32_t)g_d2client_11b9a4, Hh = (int32_t)g_d2client_dbc4c, W = (int32_t)g_d2client_dbc48;
    const int32_t x = (int32_t)u16w(msg + 0xc), y = (int32_t)u16w(msg + 0xe);
    if (!(x < X0 + 0x80) && !(x > X0 + 0xa0) && !(y < Y0 + Hh - 0x5c) && !(y > Y0 + Hh - 0x3c)) {
        g_d2client_11bbc4 = 1;
        e.wr32(msg + 0x18, 1); e.wr32(msg + 0x1c, 0);
        e.h(0x88A70, 0, 0, 0, 4);
        return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
    }
    if (!(x < X0) && !(x > ((W - (W >> 31)) >> 1) - 1) && !(y < Y0) && !(y > Y0 + Hh - 0x31)) {
        if (e.h(0xC250, 0, 0, 0, 0, 0, g_d2client_11bbfc, 4, 0)) {
            for (int32_t k = 0; k < 4; k++) {
                uint8_t* row = g_d2client_dd810 + k * 0xe;
                int32_t ey = (int32_t)b32(row + 4) + Y0 + Hh;
                if (y <= ey - 0x1f6 || y >= ey - 0x1e0) continue;
                int32_t ex = (int32_t)b32(row) + X0;
                if (x <= ex || x >= ex + 0x28) continue;
                row[8] = 1; row[9] = 0; row[10] = 0; row[11] = 0;
                e.h(0x16360);
                e.wr32(msg + 0x18, 1); e.wr32(msg + 0x1c, 0);
                e.h(0x88A70, 0, 0, 0, 5);
                return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
            }
        }
        e.wr32(msg + 0x18, 1); e.wr32(msg + 0x1c, 0);
        return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
    }
    const uint32_t r = e.h(0xBCB30);
    if (!r) e.wr32(msg + 0x18, 0);
    return r;
}
}
