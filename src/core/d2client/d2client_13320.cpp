// 1.13c D2Client+0x13320 -- stdcall key message handler. With the busy flag 0x11C3A4 set the message's +0x18 is cleared and the message pointer returned. Otherwise, when the armed flag 0xF8BEC is clear it is set to 1; when it is set and the message's +0xC has bit 30 clear (not an auto-repeat), 0x131D0 runs and then 0x88A70 with EBX = 1 and four zero stack words. The message is marked handled (+0x18 = 1, +0x1C = 0) and passed to Storm ordinal 511, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c3a4;
__attribute__((weak)) uint32_t g_d2client_f8bec;
namespace d2cl {
uint32_t f13320(GE& e, uint32_t msg)
{
    if (g_d2client_11c3a4) { e.wr32(msg + 0x18, 0); return msg; }
    if (!g_d2client_f8bec) g_d2client_f8bec = 1;
    else if (!(e.rd32(msg + 0xc) & 0x40000000)) {
        e.h(0x131D0);
        e.h(0x88A70, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    }
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
