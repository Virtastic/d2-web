// 1.13c D2Client+0x122B0 -- stdcall message handler. While the dword 0x11C3A4 is set the message is marked unhandled (+0x18 = 0) and returned. Otherwise a clear dword 0xF8BEC is set to 1; a set one, for a message whose dword +0xC lacks bit 0x40000000, runs 0x38CB0 (ECX = the dword 0xF8BE8, EDX = the dword at 10 * the index 0x11C538 in the table the pointer 0xF8B94 names) and 0x88A70 (EBX = 2, four zero stack words). The message is then marked handled (+0x18 = 1, +0x1C = 0) and passed to Storm ordinal 511, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c3a4;
__attribute__((weak)) uint32_t g_d2client_f8bec;
__attribute__((weak)) uint32_t g_d2client_f8be8;
__attribute__((weak)) uint32_t g_d2client_11c538;
namespace d2cl {
uint32_t f122b0(GE& e, uint32_t, uint32_t msg, uint32_t tb)
{
    if (g_d2client_11c3a4) {
        e.wr32(msg + 0x18, 0);
        return msg;
    }
    if (!g_d2client_f8bec) {
        g_d2client_f8bec = 1;
    } else if (!(e.rd32(msg + 0xc) & 0x40000000)) {
        e.h(0x38CB0, 0, g_d2client_f8be8, e.rd32(tb + g_d2client_11c538 * 10));
        e.h(0x88A70, 0, 0, 0, 2, 0, 0, 0, 0, 0);
    }
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
