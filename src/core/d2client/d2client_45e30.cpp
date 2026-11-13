// 1.13c D2Client+0x45E30 -- ECX = unit (may be null). A unit whose class (+4) is 0x13E and whose mode (+0x10) is 2 sets the byte 0x11C33E to 1. EAX is left alone.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c33c;
namespace d2cl {
uint32_t f45e30(GE& e, uint32_t eax, uint32_t u)
{
    if (u && e.rd32(u + 4) == 0x13e && e.rd32(u + 0x10) == 2) g_d2client_11c33c = (g_d2client_11c33c & 0xff00ffffu) | 0x10000;
    return eax;
}
}
