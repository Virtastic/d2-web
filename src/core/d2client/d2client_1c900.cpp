// 1.13c D2Client+0x1C900 -- ECX = unit (may be null). A unit whose class (+4) is 0x20F has bit 1 of its flags (+0xC4) cleared. EAX is left alone.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
uint32_t f1c900(GE& e, uint32_t eax, uint32_t u)
{
    if (u && e.rd32(u + 4) == 0x20f) e.wr32(u + 0xc4, e.rd32(u + 0xc4) & ~2u);
    return eax;
}
}
