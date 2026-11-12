// 1.13c D2Launch+0xC610 -- stdcall message handler (list keys): when Fog 10227 answers non-zero the worker 0xC010 gets the message in EAX and its answer is returned. A message whose +4 word is 0x102 returns 0. Otherwise the key at +8 moves the selection (image offset 0x24DB0) and the first visible line (0x25ED4) within the line count (0x25ED0) and the page size (0x24DBC): 0x23 end, 0x24 home, 0x26 up, 0x28 down; then the workers 0xB600 and 0xA8C0 run and the handler returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_24dbc;
__attribute__((weak)) uint32_t g_d2launch_25ed4;
__attribute__((weak)) uint32_t g_d2launch_25ed0;
__attribute__((weak)) uint32_t g_d2launch_24db0;
namespace d2launch {
uint32_t fc610(GE& e, uint32_t m)
{
    if (e.h(0x980e)) return e.h(0xC010, m);
    if (e.rd32(m + 4) == 0x102) return 0;
    int32_t page = (int32_t)g_d2launch_24dbc, cnt = (int32_t)g_d2launch_25ed0, sel = (int32_t)g_d2launch_24db0, top = (int32_t)g_d2launch_25ed4;
    bool chk = false;
    switch (e.rd32(m + 8)) {
    case 0x23:
        if (cnt > page) g_d2launch_25ed4 = (uint32_t)(cnt - page);
        g_d2launch_24db0 = (uint32_t)(cnt - 1);
        break;
    case 0x24:
        g_d2launch_25ed4 = 0;
        g_d2launch_24db0 = 0;
        break;
    case 0x26:
        if (sel <= 0) break;
        g_d2launch_24db0 = (uint32_t)--sel;
        if (sel < top) g_d2launch_25ed4 = (uint32_t)sel; else chk = true;
        break;
    case 0x28:
        if (sel >= cnt - 1) break;
        g_d2launch_24db0 = (uint32_t)++sel;
        if (sel < top) g_d2launch_25ed4 = (uint32_t)sel; else chk = true;
        break;
    default: break;
    }
    if (chk && sel > page + top - 1) g_d2launch_25ed4 = (uint32_t)(sel - page + 1);
    e.h(0xB600);
    e.h(0xA8C0);
    return 1;
}
}
