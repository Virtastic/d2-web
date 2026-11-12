// 1.13c D2Multi+0xC470 -- stdcall, one ignored argument: runs the workers 0x86B0 and 0xBE00 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2multi {
uint32_t fc470(GE& e, uint32_t)
{
    e.h(0x86B0);
    e.h(0xBE00);
    return 1;
}
}
