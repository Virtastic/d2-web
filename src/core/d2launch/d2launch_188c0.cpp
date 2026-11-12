// 1.13c D2Launch+0x188C0 -- stdcall message handler: calls D2Win 10088, then D2Win 10005 with the two module callbacks (image offsets 0x15890, 0x15820), 0 and the launch record pointer (0x25ABC); when D2Sound 10035 answers non-zero calls D2Sound 10045 with ECX = 1; runs the worker 0x17DF0 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25abc;
namespace d2launch {
uint32_t f188c0(GE& e, uint32_t)
{
    e.h(0x98da);
    e.h(0x98fe, 0, 0xB2015890u, 0xB2015820u, 0, 0, 0, g_d2launch_25abc);
    if (e.h(0x9a4e)) e.h(0x9a72, 0, 1);
    e.h(0x17DF0);
    return 1;
}
}
