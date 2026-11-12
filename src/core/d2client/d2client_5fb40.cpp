// 1.13c D2Client+0x5FB40 -- One stack argument (unused). Adds 16 to the dword 0x11C1EC. EAX is left alone.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c1ec;
namespace d2cl {
uint32_t f5fb40(GE& e, uint32_t eax, uint32_t)
{
    g_d2client_11c1ec += 0x10;
    return eax;
}
}
