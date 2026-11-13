// 1.13c D2Game+0x57270 -- sets one quest flag chosen by the message id 0x4ECF..0x4ED4
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f57270(NE& e, uint32_t q, uint32_t a)
{
    uint32_t eax = NE::rd16(a + 0x18) + 0xffffb131u;
    if (eax > 5) return eax;
    static const uint8_t flag[6] = { 7, 9, 5, 4, 6, 8 };
    const uint32_t pd = e.imp(0xA4C0, e.rd32(a + 0xc));
    return e.setf(e.rd32(pd + 0x10 + 4 * e.rd8(e.rd32(q + 4) + 0x6d)), e.rd32(q + 0xe0), flag[eax]);
}
}
