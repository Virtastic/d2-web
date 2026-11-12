// 1.13c D2Client+0x633E0 -- ECX = menu item. Saves the item's value (+0x124) through Storm ordinal 426 (the key at 0xD3588, the name at 0xD53A0, 0, value), stores it in the dword 0xF16BC and returns the Storm result.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_f16bc;
namespace d2cl {
uint32_t f633e0(GE& e, uint32_t it)
{
    const uint32_t v = e.rd32(it + 0x124);
    const uint32_t r = e.h(0xBEF6, 0, 0, 0, 0, 0, 0xB20D3588u, 0xb20d53a0u, 0, v);
    g_d2client_f16bc = v;
    return r;
}
}
