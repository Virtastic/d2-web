// 1.13c D2Game+0xB31D0 -- path retarget step: caps the path length, reseeds from the path value plus the argument, and four times in five restarts the path
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t fb31d0(NE& e, uint32_t eax0, uint32_t u, uint32_t v)
{
    if (!u) return eax0;
    uint32_t len = e.imp(0xAD06, u);
    if ((int32_t)len >= 0x4e) { len = 0x4d; e.imp(0xAD42, u, 0x4d); e.imp(0xAC7C, u, 0x4d); }
    const uint32_t path = e.rd32(u + 0x2c);
    e.h(0xA628, 0, u + 0x20, e.imp(0xAAC6, path) + v);
    const uint64_t s = (uint64_t)e.rd32(u + 0x20) * 0x6ac690c5u + e.rd32(u + 0x24);
    e.wr32(u + 0x24, (uint32_t)(s >> 32)); e.wr32(u + 0x20, (uint32_t)s);
    if ((uint32_t)s % 100 < 0x14) return (uint32_t)s / 100;
    e.imp(0xAAFC, path, 0xa);
    e.imp(0xAB14, path, len);
    return e.imp(0xAAF0, path, u, 0);
}
}
