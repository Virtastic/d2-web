// 1.13c D2Client+0x12400 -- stdcall message handler. While the dword 0x11C3A4 is set the message is marked unhandled (+0x18 = 0) and returned. Otherwise the selection step 0x117C0 (ECX = 1) repeats while the first dword of the selected row (10 * the index 0x11C538 into the table the pointer 0xF8B94 names, re-read after each step) is 0x39; when the index differs from its value on entry 0x88A70 runs (EBX = 1, four zero stack words). The message is marked handled (+0x18 = 1, +0x1C = 0) and passed to Storm ordinal 511, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c3a4;
__attribute__((weak)) uint32_t g_d2client_11c538;
namespace d2cl {
uint32_t f12400(GE& e, uint32_t msg, uint32_t tb)
{
    if (g_d2client_11c3a4) { e.wr32(msg + 0x18, 0); return msg; }
    const uint32_t was = g_d2client_11c538;
    do {
        e.h(0x117C0, 0, 1);
    } while (e.rd32(tb + g_d2client_11c538 * 10) == 0x39);
    if (was != g_d2client_11c538) e.h(0x88A70, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
