// 1.13c D2Net+0x6300 -- ECX = packet bytes, EDX = size, six stack arguments (ret 0x18): the first four are out pointers (length, 0, class, 100). Asks the length routine 0x65D0 for the
// packet's length (a 16-bit out value). Returns 3 on size 0, an unknown length, or a length above the size; 4 on a first byte in 0x71..0xFE or a length above 0x204; otherwise writes
// *s1 = 0, *s0 = length, *s2 = class (first byte < 0x67 -> 1, < 0x71 -> 0, 0xFF -> 2), *s3 = 100 and returns 2 when the class is 0, else 1. The class is also read back from *s2.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2n {
uint32_t f6300(GE& e, uint32_t buf, uint32_t size, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3)
{
    if (size < 1) return 3;
    const uint8_t b = *(const uint8_t*)(uintptr_t)buf;
    uint32_t local = 0;
    const uint32_t r = e.h(0x65D0, 0, buf, size, 0, 0, e.stk(&local));
    if (!r) return 3;
    if (b >= 0x71 && b != 0xFF) return 4;
    if ((uint16_t)local > 0x204) return 4;
    const uint32_t w = (uint16_t)local;
    if ((int32_t)w > (int32_t)size) return 3;
    uint32_t cls;
    if (b < 0x67) cls = 1;
    else cls = b < 0x71 ? 0u : 2u;
    HookEnv::wr32(s1, 0);
    HookEnv::wr32(s0, w);
    HookEnv::wr32(s2, cls);
    HookEnv::wr32(s3, 0x64);
    return cls == 0 ? 2 : 1;
}
}
