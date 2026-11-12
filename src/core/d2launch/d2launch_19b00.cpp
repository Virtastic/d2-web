// 1.13c D2Launch+0x19B00 -- stdcall message handler (the D2x_Intro video): calls D2Win 10088, then D2Win 10005 with the two module callbacks, 0 and the launch record pointer (0x25ABC), once more when it answers 0 (its answer in place of the 0); then the worker 0x199C0 with (the message argument, the video path format). Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25abc;
namespace d2launch {
uint32_t f19b00(GE& e, uint32_t m)
{
    e.h(0x98da);
    const uint32_t v = e.h(0x98fe, 0, 0xB2015890u, 0xB2015820u, 0, 0, 0, g_d2launch_25abc);
    if (!v) e.h(0x98fe, 0, 0xB2015890u, 0xB2015820u, 0, 0, v, g_d2launch_25abc);
    e.h(0x199C0, 0, 0, 0, 0, 0, m, 0xB2000000u | 0x1e1bc);
    return 1;
}
}
