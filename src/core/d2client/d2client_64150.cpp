// 1.13c D2Client+0x64150 -- ECX = menu item. 0x63620 (EAX = 0xFF, ECX = 0x37, EDX = item) gives the slider value, which is saved through Storm ordinal 426 (the key at 0xD3588, the name at 0xD51C8, 0, value) and handed to D2Gfx ordinal 10034, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
uint32_t f64150(GE& e, uint32_t it)
{
    const uint32_t v = e.h(0x63620, 0xff, 0x37, it);
    e.h(0xBEF6, 0, 0, 0, 0, 0, 0xB20D3588u, 0xB20D51C8u, 0, v);
    return e.h(0xD13E, 0, 0, 0, 0, 0, v);
}
}
