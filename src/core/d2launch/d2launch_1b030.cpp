// 1.13c D2Launch+0x1B030 -- stdcall message handler: stores 0 at image offset 0x25AD4; when the worker 0x11F30 answers 1 calls D2Sound 10019 and the workers 0x15D30 and 0xD5D0, otherwise the worker 0x16E80. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25ad4;
namespace d2launch {
uint32_t f1b030(GE& e, uint32_t)
{
    g_d2launch_25ad4 = 0;
    if (e.h(0x11F30) == 1) { e.h(0x9a5a); e.h(0x15D30); e.h(0xD5D0); }
    else e.h(0x16E80);
    return 1;
}
}
