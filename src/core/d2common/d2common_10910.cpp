// 1.13c D2Common+0x10910 -- list walk (ecx = the owner, list head at +0x10, next at +0x24): returns the node with the smallest value at +0x34 among those that pass: bit 1 of the flags word (+0x20, +0xC) clear, every entry of the node chain (+0, next at +0x14) with a non-zero word at +4, and a trial room (worker 0x44BD0 on the node level, type 2, sized from the level preset) that the worker 0xEEC0 can place beside the node. A placed trial is linked both ways (worker 0x44950), added to the level room list and count, announced (worker 0xE490) and released again (worker 0x44F90); a trial that cannot be placed is only released.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
uint32_t f10910(GE& e, uint32_t c)
{
    uint32_t best = 0;
    for (uint32_t n = e.rd32(c + 0x10); n; n = e.rd32(n + 0x24)) {
        if (best && (int32_t)e.rd32(n + 0x34) >= (int32_t)e.rd32(best + 0x34)) continue;
        if (e.rd32(e.rd32(n + 0x20) + 0xc) & 2) continue;
        bool open = false;
        for (uint32_t q = e.rd32(n); q; q = e.rd32(q + 0x14))
            if (!e.rd32(q + 4)) { open = true; break; }
        if (open) continue;
        const uint32_t x = e.h(0x44bd0, 0, 0, 0, e.rd32(n + 0x58), 0, 2);
        const uint32_t l = e.rd32(e.rd32(x + 0x58) + 0x14);
        e.wr32(x + 0x3c, e.rd32(l + 0x10));
        e.wr32(x + 0x40, e.rd32(l + 0x14));
        if (!e.h(0xeec0, 0, n, x)) {
            e.h(0x44f90, 0, 0, 0, 0, 0, x);
            continue;
        }
        e.h(0x44950, x, 0, 0, 0, n, 0);
        e.h(0x44950, n, 0, 0, 0, x, 2);
        const uint32_t lv = e.rd32(n + 0x58);
        e.wr32(x + 0x24, e.rd32(lv + 0x10));
        const uint32_t cnt = e.rd32(lv + 8) + 1;
        e.wr32(lv + 0x10, x);
        e.wr32(lv + 8, cnt);
        e.h(0xe490, 0, 0, 0, x, 0, 1);
        e.h(0x44f90, 0, 0, 0, 0, 0, x);
        best = n;
    }
    return best;
}
}
