// 1.13c D2Launch+0xC340 -- stdcall, one ignored argument: copies the module string at image offset 0x25758 over the one at 0x259A0 through the Storm import 0x9790 (destination, source, 0x7FFFFFFF), runs the worker 0xBC20 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2launch {
uint32_t fc340(GE& e, uint32_t)
{
    e.h(0x9790, 0, 0, 0, 0, 0, 0xB20259A0u, 0xB2025758u, 0x7fffffff);
    e.h(0xBC20);
    return 1;
}
}
