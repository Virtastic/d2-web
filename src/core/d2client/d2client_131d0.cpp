// 1.13c D2Client+0x131D0 -- No arguments. Copies the 0x474 bytes at 0x119E68 over 0x11A2E0, runs 0x3A3A0, takes and clears the dword 0xFADAC (Fog ordinal 10227 is asked in between; the dword 0xDD5D0 must be 0x26, retail halts otherwise). A taken non-zero value: when 0x42090 or 0x420E0 answers non-zero, 0x3CC60 runs with EAX = 1 and the stack words (D2Gfx ordinal 10048, the string at 0xD0774); then 0x3CC60 with EAX = 0xD and (D2Gfx ordinal 10048, the string at 0xD06D8), and 0x12DD0 (0). Then 0x11F80 and 0x65760 (EAX = 0, stack 1), whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_fadac;
__attribute__((weak)) uint32_t g_d2client_dd5d0;
__attribute__((weak)) uint8_t g_d2client_119e68[1140];
__attribute__((weak)) uint8_t g_d2client_11a2e0[1140];
namespace d2cl {
uint32_t f131d0(GE& e)
{
    __builtin_memcpy(g_d2client_11a2e0, g_d2client_119e68, 0x474);
    e.h(0x3A3A0);
    const uint32_t was = g_d2client_fadac;
    e.h(0xBF8C);
    g_d2client_fadac = 0;
    if (was) {
        if (e.h(0x42090) || e.h(0x420E0)) {
            const uint32_t w = e.h(0xD0B4);
            e.h(0x3CC60, 1, 0, 0, 0, 0, w, 0xB20D0774u);
        }
        const uint32_t w = e.h(0xD0B4);
        e.h(0x3CC60, 0xd, 0, 0, 0, 0, w, 0xB20D06D8u);
        e.h(0x12DD0, 0, 0, 0, 0, 0, 0);
    }
    e.h(0x11F80);
    return e.h(0x65760, 0, 0, 0, 0, 0, 1);
}
}
