// 1.13c D2Launch+0x19B50 -- stdcall message handler (the Act04end video): calls D2Win 10088, then D2Win 10173 with ECX = the module callback at image offset 0x157B0, EDX = 0 and one zero stack word. When that answers 0: D2Win 10088 again, D2Win 10005 with the two module callbacks, 0 and the launch record pointer (0x25ABC), and the handler returns 0. Otherwise the worker 0x199C0 runs with (the message argument, the video path format) and the handler returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25abc;
namespace d2launch {
uint32_t f19b50(GE& e, uint32_t m)
{
    e.h(0x98da);
    if (!e.h(0x99b2, 0, 0xB20157B0u, 0, 0, 0, 0)) {
        e.h(0x98da);
        e.h(0x98fe, 0, 0xB2015890u, 0xB2015820u, 0, 0, 0, g_d2launch_25abc);
        return 0;
    }
    e.h(0x199C0, 0, 0, 0, 0, 0, m, 0xB2000000u | 0x1dcac);
    return 1;
}
}
