// 1.13c D2Client+0x124E0 -- stdcall mouse-wheel message handler. With the busy flag 0x11C3A4 set the message's +0x18 is cleared and the message pointer returned. Otherwise the signed word at +0xA divided by 120 (truncating) is the notch count; a non-zero count calls 0x11360 with ECX = -2 * count and then 0x88A70 with EBX = 1 and four zero stack words. The message is marked handled (+0x18 = 1, +0x1C = 0) and passed to Storm ordinal 511, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c3a4;
namespace d2cl {
uint32_t f124e0(GE& e, uint32_t msg)
{
    if (g_d2client_11c3a4) { e.wr32(msg + 0x18, 0); return msg; }
    const int32_t n = e.rd16s(msg + 0xa) / 120;
    if (n) {
        e.h(0x11360, 0, (uint32_t)(-n) * 2);
        e.h(0x88A70, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    }
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
