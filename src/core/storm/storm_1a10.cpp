// 1.13c Storm+0x1A10 -- ECX = the object. Two register-convention callees in a row: 0x20C0 (EBX = object, one stack
// word 0) and 0x2080 (EDI = object); the second one's answer is what EAX holds at return.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace storm {
uint32_t f1a10(GE& e, uint32_t self)
{
    e.h(0x20C0, 0, 0, 0, self, 0, 0);
    const uint32_t v = e.h(0x2080);
    e.edi(0x2080, self);
    return v;
}
}
