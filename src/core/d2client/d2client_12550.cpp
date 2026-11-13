// 1.13c D2Client+0x12550 -- stdcall mouse-down message handler. While the dword 0x11C3A4 is set the message is marked unhandled (+0x18 = 0) and returned. Otherwise the press token 0x11C508 is set to 1, 0x11A80 runs with the message's word +0xC then word +0xE as its two stack words, the message is marked handled (+0x18 = 1, +0x1C = 0) and passed to Storm ordinal 511, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c3a4;
__attribute__((weak)) uint32_t g_d2client_11c508;
namespace d2cl {
uint32_t f12550(GE& e, uint32_t msg)
{
    if (g_d2client_11c3a4) { e.wr32(msg + 0x18, 0); return msg; }
    g_d2client_11c508 = 1;
    e.h(0x11A80, 0, 0, 0, 0, 0, e.rd32(msg + 0xc) & 0xFFFF, (e.rd32(msg + 0xc) >> 16));
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
