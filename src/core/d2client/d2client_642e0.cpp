// 1.13c D2Client+0x642E0 -- ECX = menu item. With the sound flag 0x11C280 clear the item's value (+0x124) becomes 0. Otherwise D2Sound ordinal 10059 is asked and the value becomes 1 when it answers 2, 0 otherwise. The value is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c280;
namespace d2cl {
uint32_t f642e0(GE& e, uint32_t it)
{
    uint32_t v = 0;
    if (g_d2client_11c280) {
        const uint32_t r = e.h(0xD324);
        v = r == 2;
    }
    e.wr32(it + 0x124, v);
    return v;
}
}
