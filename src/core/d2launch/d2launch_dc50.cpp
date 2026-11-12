// 1.13c D2Launch+0xDC50 -- stdcall message handler (convert the selected character to expansion): does nothing unless Fog 10227 answers non-zero. Walks the character list (image offset 0x25EC8, next at +0x34C) to the selection (0x24DB0); none goes to the common tail. A node already flagged 0x20 at +0x324 returns 1. With a realm name at +0x100 it is narrowed and the worker 0x16310 asked; without one the worker 0xF030: non-zero sets the flag and runs the workers 0xB600 and 0x12B60. A refusal with a realm name asks D2MCPClient 10036: 0x7B formats string 0x2B89 with the character name into a message box (worker 0x15090), anything else shows string 0x145B, and both clear the flag at 0x25EAC and return 1. A refusal without one shows string 0x5570 through the worker 0x138F0. The tail is D2Win 10147 on the control at 0x25998 with 0; returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25ec8;
__attribute__((weak)) uint32_t g_d2launch_24db0;
__attribute__((weak)) uint32_t g_d2launch_25eac;
__attribute__((weak)) uint32_t g_d2launch_25998;
namespace d2launch {
static uint32_t rd16(GE& e, uint32_t a) { return e.rd8(a) | (e.rd8(a + 1) << 8); }
static void wr16(GE& e, uint32_t a, uint32_t v) { e.wr8(a, v & 0xff); e.wr8(a + 1, (v >> 8) & 0xff); }
uint32_t fdc50(GE& e, uint32_t)
{
    if (!e.h(0x980e)) return 1;
    uint32_t n = g_d2launch_25ec8;
    for (uint32_t i = 0; n && i != g_d2launch_24db0; i++) n = e.rd32(n + 0x34c);
    if (n) {
        if (e.rd8(n + 0x324) & 0x20) return 1;
        uint8_t buf[0x500];
        bool ok;
        if (rd16(e, n + 0x100)) {
            e.h(0x1c03c, 0, e.stk(buf), n + 0x100, 0, 0, 0xff);
            ok = e.h(0x16310, 0, 0, 0, 0, 0, n) != 0;
            if (!ok) {
                if (e.h(0x9ad8) != 0x7b) {
                    e.h(0x15090, 0, 0, 0, 0, 0, e.h(0x9b32, 0, 0x145b), 0xB200d5d0u, 1);
                } else {
                    e.h(0x1c038, 0, e.stk(buf), n, 0, 0, 0x100);
                    const uint32_t f = e.h(0x9b32, 0, 0x2b89);
                    e.h(0x1c04c, 0, 0, 0, 0, 0, 0x100, e.stk(buf), f, e.stk(buf));
                    e.h(0x15090, 0, 0, 0, 0, 0, e.stk(buf), 0xB200d5d0u, 1);
                }
                g_d2launch_25eac = 0;
                return 1;
            }
        } else {
            ok = e.h(0xF030, 0, 0, 0, 0, 0, n) != 0;
            if (!ok) e.h(0x138F0, 0, 0, 0, 0, 0, e.h(0x9b32, 0, 0x5570), 0xB200d5d0u, 1);
        }
        if (ok) {
            wr16(e, n + 0x324, rd16(e, n + 0x324) | 0x20);
            e.h(0xB600);
            e.h(0x12B60);
        }
    }
    e.h(0x9862, 0, g_d2launch_25998, 0);
    return 1;
}
}
