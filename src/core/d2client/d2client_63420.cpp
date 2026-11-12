// 1.13c D2Client+0x63420 -- ECX = menu item. Copies the dword 0xF16B8 into the item's value (+0x124) and returns it.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_f16b8;
namespace d2cl {
uint32_t f63420(GE& e, uint32_t it)
{
    e.wr32(it + 0x124, g_d2client_f16b8);
    return g_d2client_f16b8;
}
}
