// 1.13c D2Launch+0x1A220 -- stdcall, no arguments: worker 0x11950 (ECX = the control at image offset 0x254FC, EDI = the word at 0x2529C); the install path (D2Win 10115 into a 0x400 local) is cut after its first backslash and handed to GetDiskFreeSpaceA; free * bytes-per-sector * sectors (32-bit) of 5000 or more returns the worker 0x18E10, less shows the D2Win 10003 text 0x141D through the worker 0x15090 (callback 0x19FA0, 1) and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_254fc;
__attribute__((weak)) uint32_t g_d2launch_2529c;
namespace d2launch {
uint32_t f1a220(GE& e, uint32_t)
{
    e.h(0x11950, 0, g_d2launch_254fc);
    e.j(0x11950, g_d2launch_2529c);
    char buf[0x400] = {};
    e.h(0x97cc, 0, e.stk(buf), 0x400);
    for (uint32_t i = 0; i < 0x400; i++)
        if (buf[i] == '\\') { buf[i + 1] = 0; break; }
    uint32_t spc = 0, bps = 0, fr = 0, tot = 0;
    e.hs(0x1c354, 0, 0, 0, 0, 0, { e.stk(buf), e.stk(&spc), e.stk(&bps), e.stk(&fr), e.stk(&tot) });
    if (fr * bps * spc >= 5000) return e.h(0x18e10);
    const uint32_t t = e.h(0x9b32, 0, 0x141d);
    e.h(0x15090, 0, 0, 0, 0, 0, t, 0xB2019fa0u, 1);
    return 1;
}
}
