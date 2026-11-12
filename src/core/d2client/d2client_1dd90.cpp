// 1.13c D2Client+0x1DD90 -- stdcall mouse message handler. 0x1D840 is asked with the message's words at +0xC and +0xE (x, y). A zero low byte clears the byte 0x1223F0 and the message's +0x18 and returns 0. Otherwise the byte 0x1223F0 is set to 1 and 0x88A70 runs with EBX = 4 and four zero stack words. The message is marked handled (+0x18 = 1, +0x1C = 0) and passed to Storm ordinal 511, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_1223f0;
static inline uint32_t m16(uint32_t a) { uint16_t v; __builtin_memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
namespace d2cl {
uint32_t f1dd90(GE& e, uint32_t msg)
{
    if (!(e.h(0x1D840, 0, 0, 0, 0, 0, m16(msg + 0xc), m16(msg + 0xe)) & 0xff)) {
        g_d2client_1223f0 &= ~0xffu;
        e.wr32(msg + 0x18, 0);
        return 0;
    }
    g_d2client_1223f0 = (g_d2client_1223f0 & ~0xffu) | 1;
    e.h(0x88A70, 0, 0, 0, 4, 0, 0, 0, 0, 0);
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
