// 1.13c D2Launch+0x1AFA0 -- stdcall message handler: stores 8 at +0x15 of the launch record (pointer at 0x25ABC), copies the string Fog 10013 returns to +0x33, stores 2 at image offset 0x25AD4 and 4 at +0x205 of the record. When the worker 0x11F30 answers 0 runs the worker 0x16E80; otherwise D2Sound 10019 and the workers 0x15D30 and 0xD5D0. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25abc;
__attribute__((weak)) uint32_t g_d2launch_25ad4;
namespace d2launch {
uint32_t f1afa0(GE& e, uint32_t)
{
    const uint32_t rec = g_d2launch_25abc;
    e.wr32(rec + 0x15, 8);
    uint32_t s = e.h(0x97c6), d = rec + 0x33;
    for (;;) { const uint32_t c = e.rd8(s++); e.wr8(d++, c); if (!c) break; }
    g_d2launch_25ad4 = 2;
    e.wr32(rec + 0x205, 4);
    if (!e.h(0x11F30)) { e.h(0x16E80); return 1; }
    e.h(0x9a5a);
    e.h(0x15D30);
    e.h(0xD5D0);
    return 1;
}
}
