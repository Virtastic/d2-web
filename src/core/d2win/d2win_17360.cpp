// 1.13c D2Win+0x17360 -- stdcall(msg): window activation (WM_ACTIVATEAPP-style) handler. msg[2] is the active flag.
// Active: if 0x7696 answers 0, 0x7792(0), mark 0xC9E28 = 1, stamp 0xC9E2C with GetTickCount, 0x41630 = 2, 0x1FD44 = 1 and 0x7858(&DAT 0x42338); then 0x7720(ecx = 0xC9CF8), 0xC9CFC = 0.
// Inactive: when 0xC9CFC is clear, 0xC9CF8 = 0x771a(); 0xC9CFC = 1; if the mode word 0x21498 is not 1, 0x7792(1). Both: 0x77b6() nonzero calls 0x77f8(); msg[6] = 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2win_1fba8, g_d2win_c9cf8, g_d2win_c9cfc, g_d2win_c9e28, g_d2win_c9e2c, g_d2win_41630, g_d2win_1fd44, g_d2win_21498;
namespace d2w {
void f17360(GE& e, uint32_t* msg, uint32_t tick)
{
    g_d2win_1fba8 = msg[2] != 0;
    if (msg[2]) {
        if (!e.h(0x7696, 0, 0, 0, 0, 0)) {
            e.h(0x7792, 0, 0, 0, 0, 0, 0);
            g_d2win_c9e28 = 1;
            g_d2win_c9e2c = tick;
            g_d2win_41630 = 2;
            g_d2win_1fd44 = 1;
            e.h(0x7858, 0, 0, 0, 0, 0, 0xB2000000u | 0x42338u);
        }
        e.h(0x7720, 0, g_d2win_c9cf8, 0, 0, 0);
        g_d2win_c9cfc = 0;
    } else {
        if (!g_d2win_c9cfc) g_d2win_c9cf8 = e.h(0x771a, 0, 0, 0, 0, 0);
        g_d2win_c9cfc = 1;
        if (g_d2win_21498 != 1) e.h(0x7792, 0, 0, 0, 0, 0, 1);
    }
    if (e.h(0x77b6, 0, 0, 0, 0, 0)) e.h(0x77f8, 0, 0, 0, 0, 0);
    msg[6] = 0;
}
}
