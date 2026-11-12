// 1.13c D2CMP+0x1380 -- ECX = the object. Two register-convention callees in a row: 0x1880 (EBX = object, one stack
// word 0) and 0x1840 (EDI = object); the second one's answer is what EAX holds at return.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cmp {
uint32_t f1380(GE& e, uint32_t self)
{
    e.h(0x1880, 0, 0, 0, self, 0, 0);
    const uint32_t v = e.h(0x1840);
    e.edi(0x1840, self);
    return v;
}
}
