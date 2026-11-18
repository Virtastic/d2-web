// 1.13c D2Launch+0x19950 -- stdcall, one ignored argument: when the D2Win import 0x98B6 answers non-zero the intro at
// 0x19680 runs; then the D2Win import 0x992E takes 1 and the screen worker at 0x18160 runs. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2launch {
uint32_t f19950(GE& e, uint32_t)
{
    if (e.h(0x98B6)) e.h(0x19680);
    e.h(0x992E, 0, 0, 0, 0, 0, 1);
    e.h(0x18160);
    return 1;
}
}
