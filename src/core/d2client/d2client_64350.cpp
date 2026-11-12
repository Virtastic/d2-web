// 1.13c D2Client+0x64350 -- ECX = menu item. 0x63620 (EAX = 0x64, ECX = 0, EDX = item) gives the slider value, which is passed in ECX to D2Sound ordinal 10022 (a tail jump); that result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
uint32_t f64350(GE& e, uint32_t it)
{
    return e.h(0xD282, 0, e.h(0x63620, 0x64, 0, it));
}
}
