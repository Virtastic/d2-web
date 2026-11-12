// 1.13c D2Game+0xBCBE0 -- Two stack arguments: a unit and a byte flag. When the D2Common import 0xA42A(unit, 6, 0) is below 0xA3A0(unit) (signed), 0xA472(unit, 6, that, 0) runs and the flag becomes 1. A non-zero answer from the helper 0xBC920 (EBX = unit) sets the flag. Then for 0xA5D4(unit, 2) and 0xA5D4(unit, 1): a non-null answer runs 0xABB6(unit, it) and 0xABAA(it) and sets the flag. The last call's result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fbcbe0(GE& e, uint32_t u, uint32_t out)
{
    const uint32_t s = e.h(0xA42A, 0, 0, 0, 0, 0, u, 6, 0);
    const uint32_t m = e.h(0xa3a0, 0, 0, 0, 0, 0, u);
    if ((int32_t)s < (int32_t)m) {
        e.h(0xA472, 0, 0, 0, 0, 0, u, 6, m, 0);
        e.wr8(out, 1);
    }
    if (e.h(0xBC920, 0, 0, 0, u, 0)) e.wr8(out, 1);
    uint32_t v = 0;
    for (uint32_t k = 2; k >= 1; k--) {
        v = e.h(0xA5D4, 0, 0, 0, 0, 0, u, k);
        if (v) {
            e.h(0xABB6, 0, 0, 0, 0, 0, u, v);
            v = e.h(0xabaa, 0, 0, 0, 0, 0, v);
            e.wr8(out, 1);
        }
    }
    return v;
}
}
