// 1.13c D2Client+0x42660 -- stdcall mouse-wheel message handler. With the active flag 0x11C3A0 clear nothing happens and 0 is returned. Otherwise the signed word at +0xA is added to the running wheel total 0x123494; when the total's magnitude reaches 120 the total divided by 120 (truncating) is passed in EAX to 0x38FF0 and the total is cleared. The message is marked handled (+0x18 = 1, +0x1C = 0) and passed to Storm ordinal 511, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c3a0;
__attribute__((weak)) uint32_t g_d2client_123494;
namespace d2cl {
uint32_t f42660(GE& e, uint32_t msg)
{
    if (!g_d2client_11c3a0) return 0;
    const int32_t acc = (int32_t)(g_d2client_123494 + (uint32_t)e.rd16s(msg + 0xa));
    g_d2client_123494 = (uint32_t)acc;
    const int32_t sg = acc >> 31;
    if ((int32_t)(((uint32_t)acc ^ (uint32_t)sg) - (uint32_t)sg) >= 0x78) {
        e.h(0x38FF0, (uint32_t)(acc / 120));
        g_d2client_123494 = 0;
    }
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
