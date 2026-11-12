// 1.13c D2Client+0x633B0 -- ECX = menu item. The item's value (+0x124) becomes 1 when the dwords 0xF16B8 and 0xF16BC are both set, 0 otherwise; that value is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_f16b8;
__attribute__((weak)) uint32_t g_d2client_f16bc;
namespace d2cl {
uint32_t f633b0(GE& e, uint32_t it)
{
    const uint32_t v = g_d2client_f16b8 && g_d2client_f16bc;
    e.wr32(it + 0x124, v);
    return v;
}
}
