// 1.13c D2Client+0x1F900 -- stdcall mouse message handler. 0x1D840 is asked with the words at +0xC and +0xE. When its low byte is 0, or the byte 0x1223F0 is clear, that byte is cleared, the message marked unhandled (+0x18 = 0) and the 0x1D840 answer returned. Otherwise the dword 0xFADC4 is taken and cleared (Fog ordinal 10227 is asked in between; the dword 0xDD5D0 must be 0x26); a taken non-zero value runs 0x3CC60 with EAX = 4 and the stack words (D2Gfx ordinal 10048, the string at 0xD07BC), then 0x1C720. 0x1E940 (0) runs, the byte 0x1223F0 is cleared, the message marked handled (+0x18 = 1, +0x1C = 0) and passed to Storm ordinal 511, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_1223f0;
__attribute__((weak)) uint32_t g_d2client_fadc4;
__attribute__((weak)) uint32_t g_d2client_dd5d0;
static inline uint32_t m16(uint32_t a) { uint16_t v; __builtin_memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
namespace d2cl {
uint32_t f1f900(GE& e, uint32_t msg)
{
    const uint32_t r = e.h(0x1D840, 0, 0, 0, 0, 0, m16(msg + 0xc), m16(msg + 0xe));
    const bool armed = (g_d2client_1223f0 & 0xff) != 0;
    if (!(r & 0xff) || !armed) {
        g_d2client_1223f0 &= ~0xffu;
        e.wr32(msg + 0x18, 0);
        return r;
    }
    const uint32_t was = g_d2client_fadc4;
    e.h(0xBF8C);
    g_d2client_fadc4 = 0;
    if (was) {
        const uint32_t w = e.h(0xD0B4);
        e.h(0x3CC60, 4, 0, 0, 0, 0, w, 0xB20D07BCu);
        e.h(0x1C720);
    }
    e.h(0x1E940, 0, 0, 0, 0, 0, 0);
    g_d2client_1223f0 &= ~0xffu;
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
