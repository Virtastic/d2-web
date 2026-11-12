// 1.13c D2Launch+0x18770 -- stdcall message handler: when the interface table pointer (image offset 0x256C4) is set, sets the flag at 0x25EB0, clears the bytes +0x35D and +0x37D of the launch record (pointer at 0x25ABC), writes a debug line, calls D2MCPClient 10018 and the interface slot +0x10. Then runs the worker 0x17DF0 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_256c4;
__attribute__((weak)) uint32_t g_d2launch_25abc;
__attribute__((weak)) uint32_t g_d2launch_25eb0;
namespace d2launch {
uint32_t f18770(GE& e, uint32_t)
{
    if (g_d2launch_256c4) {
        g_d2launch_25eb0 = 1;
        e.wr8(g_d2launch_25abc + 0x35d, 0);
        e.wr8(g_d2launch_25abc + 0x37d, 0);
        e.h(0x9aae);
        e.h(0x9b32);
    }
    e.h(0x17DF0);
    return 1;
}
}
