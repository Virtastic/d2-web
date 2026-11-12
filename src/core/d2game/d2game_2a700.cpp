// 1.13c D2Game+0x2A700 -- EAX = whatever the caller left (returned), ECX = a record (never null), EDX = an out word: when bit 0x20 of the record's +0x3D4 byte is set the record pointer is stored in the out word.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f2a700(GE& e, uint32_t a, uint32_t p, uint32_t out)
{
    if (e.rd8(p + 0x3d4) & 0x20) e.wr32(out, p);
    return a;
}
}
