// 1.13c D2Client+0x62C30 -- stdcall, three arguments (a, buffer, size). Requires the dword 0x11C1AC to be set (retail halts otherwise). Storm ordinal 462 (a, buffer, size) is asked; on a zero answer Storm ordinal 501 copies the default string at 0xD5218 into the buffer (buffer, string, size) and 0 is returned, otherwise 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c1ac;
namespace d2cl {
uint32_t f62c30(GE& e, uint32_t a, uint32_t buf, uint32_t n)
{
    if (e.h(0xBF1A, 0, 0, 0, 0, 0, a, buf, n)) return 1;
    e.h(0xBE96, 0, 0, 0, 0, 0, buf, 0xB20D5218u, n);
    return 0;
}
}
