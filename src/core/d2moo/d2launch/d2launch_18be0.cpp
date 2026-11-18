// 1.13c D2Launch+0x18BE0 -- stdcall, one ignored argument: asks the D2Win import 0x9910 (ECX = the control global at
// image offset 0x25E50) for a count; when the global at 0x25E58 is not above count + 10 (signed) the worker at 0x17DF0
// runs. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25e50;
__attribute__((weak)) uint32_t g_d2launch_25e58;
namespace d2launch {
uint32_t f18be0(GE& e, uint32_t)
{
    const uint32_t n = e.h(0x9910, 0, g_d2launch_25e50);
    if ((int32_t)g_d2launch_25e58 <= (int32_t)(n + 10)) e.h(0x17DF0);
    return 1;
}
}
