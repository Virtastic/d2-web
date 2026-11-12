// 1.13c D2Win+0x18280 -- stdcall message handler: runs the worker 0x17EB0, then marks the message handled (+0x18 = 1, +0x1C = 0) and returns the message pointer.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2w {
uint32_t f18280(GE& e, uint32_t m)
{
    e.h(0x17EB0);
    e.wr32(m + 0x18, 1);
    e.wr32(m + 0x1c, 0);
    return m;
}
}
