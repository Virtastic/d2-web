// 1.13c D2Client+0x12050 -- stdcall message handler. With the busy flag 0x11C3A4 set the message's +0x18 is cleared and the message pointer returned. Otherwise 0x119C0 is called with EAX = 0x100. The message is marked handled (+0x18 = 1, +0x1C = 0) and passed to Storm ordinal 511, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c3a4;
namespace d2cl {
uint32_t f12050(GE& e, uint32_t msg)
{
    if (g_d2client_11c3a4) { e.wr32(msg + 0x18, 0); return msg; }
    e.h(0x119C0, 0x100);
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
