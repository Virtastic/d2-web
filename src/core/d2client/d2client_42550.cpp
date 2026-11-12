// 1.13c D2Client+0x42550 -- stdcall message handler. With the active flag 0x11C3A0 clear nothing happens and 0 is returned. Otherwise the word at +0xA picks: 1 calls 0x39120, 2 calls 0x390A0. The message is marked handled (+0x18 = 1, +0x1C = 0) and passed to Storm ordinal 511, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c3a0;
static inline uint32_t m16(uint32_t a) { uint16_t v; __builtin_memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
namespace d2cl {
uint32_t f42550(GE& e, uint32_t msg)
{
    if (!g_d2client_11c3a0) return 0;
    const uint32_t k = m16(msg + 0xa);
    if (k == 1) e.h(0x39120);
    else if (k == 2) e.h(0x390a0);
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
