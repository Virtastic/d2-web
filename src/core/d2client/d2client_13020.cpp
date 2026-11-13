// 1.13c D2Client+0x13020 -- stdcall message handler. While the dword 0x11C3A4 is set the message is marked unhandled (+0x18 = 0) and returned. Otherwise a clear dword 0xF8BEC is set to 1; a set one, for a message whose dword +0xC lacks bit 0x40000000 and whose selected row (10 * the index 0x11C538 into the table the pointer 0xF8B94 names) does not start with 0x39, runs 0x12EB0 and 0x88A70 (EBX = 2, four zero stack words) and clears the dword. The message is then marked handled (+0x18 = 1, +0x1C = 0) and passed to Storm ordinal 511, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c3a4;
__attribute__((weak)) uint32_t g_d2client_f8bec;
__attribute__((weak)) uint32_t g_d2client_11c538;
namespace d2cl {
uint32_t f13020(GE& e, uint32_t msg, uint32_t tb)
{
    if (g_d2client_11c3a4) { e.wr32(msg + 0x18, 0); return msg; }
    if (!g_d2client_f8bec) {
        g_d2client_f8bec = 1;
    } else if (!(e.rd32(msg + 0xc) & 0x40000000) && e.rd32(tb + g_d2client_11c538 * 10) != 0x39) {
        e.h(0x12EB0);
        e.h(0x88A70, 0, 0, 0, 2, 0, 0, 0, 0, 0);
        g_d2client_f8bec = 0;
    }
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
