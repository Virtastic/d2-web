// 1.13c D2Launch+0xE140 -- stdcall message handler (click in the character list): returns 1 at once when the flag at image offset 0x25984 is set. Finds the row whose control (arrays at 0x25858 when Fog 10227 answers non-zero, and at 0x25734) is the message control, among the page size (0x24DBC) rows; the row plus the first visible line (0x25ED4) beyond the line count (0x25ED0) returns 0. Otherwise it becomes the selection (0x24DB0; -1 when the list is empty). A left-button message (0x201) reads the tick count: the same valid selection clicked again within 500 ms hands the message to the worker 0xDEC0 (its answer is returned), otherwise the tick is saved at 0x25EF8. The worker 0xA8C0 runs and its answer is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25984;
__attribute__((weak)) uint32_t g_d2launch_24dbc;
__attribute__((weak)) uint32_t g_d2launch_24db0;
__attribute__((weak)) uint32_t g_d2launch_25858_a8[8];
__attribute__((weak)) uint32_t g_d2launch_25734_a8[8];
__attribute__((weak)) uint32_t g_d2launch_25ed4;
__attribute__((weak)) uint32_t g_d2launch_25ed0;
__attribute__((weak)) uint32_t g_d2launch_25ef8;
namespace d2launch {
uint32_t fe140(GE& e, uint32_t m)
{
    if (g_d2launch_25984) return 1;
    const uint32_t old = g_d2launch_24db0, id = e.rd32(m);
    int32_t i = 0;
    for (; i < (int32_t)g_d2launch_24dbc; i++) {
        if (e.h(0x980e) && id == g_d2launch_25858_a8[i]) break;
        if (id == g_d2launch_25734_a8[i]) break;
    }
    const uint32_t row = g_d2launch_25ed4 + (uint32_t)i;
    if ((int32_t)g_d2launch_25ed0 < (int32_t)row) return 0;
    g_d2launch_24db0 = row;
    if ((int32_t)g_d2launch_25ed0 <= 0) g_d2launch_24db0 = 0xffffffffu;
    if (e.rd32(m + 4) == 0x201) {
        const uint32_t t = e.h(0x1c364);
        const uint32_t sel = g_d2launch_24db0;
        if (sel != 0xffffffffu && sel == old && t - g_d2launch_25ef8 < 0x1f4) return e.h(0xDEC0, 0, 0, 0, 0, 0, m);
        g_d2launch_25ef8 = t;
    }
    return e.h(0xA8C0);
}
}
