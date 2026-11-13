// 1.13c D2Game+0x914F0 -- unit refresh after a state change: applies the state, then folds the item 0x6D charge into the stats 0xC, 6 and 7 and drops every item of slot 8
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f914f0(NE& e, uint32_t u, uint32_t v, uint32_t ebx0, uint32_t s0)
{
    e.h(0xE0460, u, 0, 0, NE::lo(ebx0, 0), 0, 1);
    e.imp(0xA3CA, u, v, 0);
    e.h(0xEFC60);
    const uint32_t it = e.imp(0xA5D4, u, 0x6d);
    if (it) {
        const uint32_t a = e.imp(0xA712, it, 0xb0, 0), b = e.imp(0xA712, it, 0xb1, 0);
        int32_t m = (int32_t)e.imp(0xA3A0, u) >> 8;
        const int32_t c = (int32_t)e.imp(0xA42A, u, 6, 0) >> 8;
        int32_t r = (int32_t)e.h(0x14D0, (uint32_t)c, (uint32_t)m, b);
        if (r <= 1) r = 1;
        if (r < m) m = r;
        e.imp(0xA472, u, 0xc, a, 0);
        e.imp(0xA472, u, 6, (uint32_t)m << 8, 0);
        e.imp(0xA472, u, 7, b << 8, 0);
        e.imp(0xABB6, u, it);
        e.imp(0xABAA, it);
    }
    uint32_t x;
    while ((x = e.imp(0xA7B4, u, 8)) != 0) { e.imp(0xABB6, u, x); e.imp(0xABAA, x); }
    return 0;
}
}
