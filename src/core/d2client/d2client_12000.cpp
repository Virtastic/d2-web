// 1.13c D2Client+0x12000 -- stdcall message handler (one argument, the message). With the busy flag 0x11C3A4 set the message's +0x18 is cleared and the message pointer returned. Otherwise the word at +0xA picks a key event: 1 calls 0x119C0 with EAX = 0x101, 2 with EAX = 0x102. The message is marked handled (+0x18 = 1, +0x1C = 0) and passed to Storm ordinal 511, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c3a4;
static inline uint32_t m16(uint32_t a) { uint16_t v; __builtin_memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
namespace d2cl {
uint32_t f12000(GE& e, uint32_t msg)
{
    if (g_d2client_11c3a4) { e.wr32(msg + 0x18, 0); return msg; }
    const uint32_t k = m16(msg + 0xa);
    if (k == 1) e.h(0x119C0, 0x101);
    else if (k == 2) e.h(0x119C0, 0x102);
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
