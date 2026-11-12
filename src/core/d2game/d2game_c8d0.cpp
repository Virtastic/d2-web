// 1.13c D2Game+0xC8D0 -- ECX = a, EDX = b, three stack arguments c, d and an ignored third: the D2Common import 0xACD0(b, c) runs, then 0x4F4D0 (ECX = a, EDX = b; stack 0x11D, d), whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fc8d0(GE& e, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t)
{
    e.h(0xACD0, 0, 0, 0, 0, 0, b, c);
    return e.h(0x4F4D0, 0, a, b, 0, 0, 0x11d, d);
}
}
