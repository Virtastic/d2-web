// 1.13c D2Client+0x71BD0 -- No arguments. Clears the dwords 0x11C020 and 0x11C01C, runs D2Win ordinal 10153 (ECX = 0), loads three cels through 0x2B420 (EAX = the name at 0xD4D18, 0xD4D18, 0xD4CF8; one zero stack word) into the dwords 0x11C000, 0x11C008 and 0x11C004, runs D2Win ordinal 10065, sets the dword 0x1197E0 to 2 and 0x1197E4 to the dword 0xF4310. With the window (D2Gfx ordinal 10048, must be non-zero) the four rows of the handler table 0xE27A0 are registered through 0x3CA80 (EAX = window, ECX = message, EDX = handler, ESI = kind), the loop 0x3CB00 runs with EBX = 0x71AD0, and the rows are unregistered through 0x3CA00 with a freshly asked window. 0x1197E0 is cleared, 0x1197E4 takes the dword 0xF4308 and 0x71360 runs. Returns 1, or 0 when the loop left the dword 0x11C020 set.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c020;
__attribute__((weak)) uint32_t g_d2client_11c01c;
__attribute__((weak)) uint32_t g_d2client_11c000;
__attribute__((weak)) uint32_t g_d2client_11c008;
__attribute__((weak)) uint32_t g_d2client_11c004;
__attribute__((weak)) uint32_t g_d2client_1197e0;
__attribute__((weak)) uint32_t g_d2client_1197e4;
__attribute__((weak)) uint32_t g_d2client_f4310;
__attribute__((weak)) uint32_t g_d2client_f4308;
// the handler table: (kind, message, handler) x 4
static const uint32_t T[4][3] = { { 0, 0x201, 0xB20718E0u }, { 0, 0x202, 0xB2071400u }, { 3, 0x1b, 0xB20710C0u }, { 3, 0xd, 0xB20710D0u } };
namespace d2cl {
uint32_t f71bd0(GE& e)
{
    uint32_t ret = 1;
    g_d2client_11c020 = 0;
    g_d2client_11c01c = 0;
    e.h(0xD3EA, 0, 0);
    g_d2client_11c000 = e.h(0x2B420, 0xB20D4D18u, 0, 0, 0, 0, 0);
    g_d2client_11c008 = e.h(0x2B420, 0xB20D4D18u, 0, 0, 0, 0, 0);
    g_d2client_11c004 = e.h(0x2B420, 0xB20D4CF8u, 0, 0, 0, 0, 0);
    e.h(0xD39C);
    g_d2client_1197e0 = 2;
    g_d2client_1197e4 = g_d2client_f4310;
    uint32_t w = e.h(0xD0B4);
    for (int i = 0; i < 4; ++i) e.h(0x3CA80, w, T[i][1], T[i][2], 0, T[i][0]);
    e.h(0x3CB00, 0, 0, 0, 0xB2071AD0u);
    if (g_d2client_11c020) ret = 0;
    w = e.h(0xD0B4);
    for (int i = 0; i < 4; ++i) e.h(0x3CA00, w, T[i][1], T[i][2], 0, T[i][0]);
    g_d2client_1197e0 = 0;
    g_d2client_1197e4 = g_d2client_f4308;
    e.h(0x71360);
    return ret;
}
}
