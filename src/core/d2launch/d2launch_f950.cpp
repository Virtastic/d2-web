// 1.13c D2Launch+0xF950 -- stdcall message handler: runs the worker 0xF800. With no saved event handle (image offset 0x25554) asks the interface table (0x256C4) slot +4 for one: none ends in D2Win 10199 and 1; one is saved and the flag at 0x25EA0 cleared. A zero-timeout wait on the handle that answers 0 calls D2Win 10199. A clear flag at 0x25EA0 then returns 1; otherwise the interface slot +0x10 is called, then D2Win 10199. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_256c4;
__attribute__((weak)) uint32_t g_d2launch_25554;
__attribute__((weak)) uint32_t g_d2launch_25ea0;
namespace d2launch {
uint32_t ff950(GE& e, uint32_t)
{
    e.h(0xF800);
    if (!g_d2launch_25554) {
        g_d2launch_25554 = e.h(0x9b32);
        if (!g_d2launch_25554) { e.h(0x994c); return 1; }
        g_d2launch_25ea0 = 0;
    }
    if (e.h(0x1c368, 0, 0, 0, 0, 0, g_d2launch_25554, 0) == 0) e.h(0x994c);
    if (!g_d2launch_25ea0) return 1;
    e.h(0x9b38);
    e.h(0x994c);
    return 1;
}
}
