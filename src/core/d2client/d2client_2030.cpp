// 1.13c D2Client+0x2030 -- thiscall, ECX = this (EDI and EBX are set to it). 0x2360(0) (EBX = this) then 0x2320() (EDI = this).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
void f2030(GE& e, uint32_t self)
{
    e.h(0x2360, 0, 0, 0, self, 0, 0);
    e.h(0x2320);
    e.edi(0x2320, self);
}
}
