// 1.13c D2Client+0x123A0 -- stdcall message handler, the mirror of 0x12340. While the dword 0x11C3A4 is set the message is marked unhandled (+0x18 = 0) and returned. Otherwise a mode dword 0xF8BE8 equal to 0 is set to 1 and 0x88A70 runs (EBX = 1, four zero stack words). The message is then marked handled (+0x18 = 1, +0x1C = 0) and passed to Storm ordinal 511, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c3a4;
__attribute__((weak)) uint32_t g_d2client_f8be8;
namespace d2cl {
uint32_t f123a0(GE& e, uint32_t msg)
{
    if (g_d2client_11c3a4) { e.wr32(msg + 0x18, 0); return msg; }
    if (g_d2client_f8be8 == 0) {
        g_d2client_f8be8 = 1;
        e.h(0x88A70, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    }
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
