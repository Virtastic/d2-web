// 1.13c D2Client+0x20D20 -- thiscall, ECX = unit (may be 0), one stack word (ret 4). With a non-zero argument 0xC5EC(unit, &kind, &local)
// fills the kind slot; a unit that is absent or has +0x10 == 0 and kind 4 reads (x, y) from 0x1630 / 0x1660 (ECX = unit) and requests
// 0xAFF10 (EDX = unit, stack code, X, Y, 0, 1) over the 3x3 window around it -- code 0x75 for (x,y), (x+1,y+1), (x+1,y-1), (x-1,y+1),
// (x-1,y-1), then code 0x52 for the same five -- and finishes with 0x88A70(unit, 0, 0, 0) with EBX = 0x99A.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
void f20d20(GE& e, uint32_t u, uint32_t arg)
{
    if (!arg) return;
    uint32_t kind = 0, local = 0;
    e.h(0xC5EC, 0, 0, 0, 0, 0, u, (uint32_t)(uintptr_t)&kind, (uint32_t)(uintptr_t)&local);
    if (u && e.rd32(u + 0x10)) return;
    if (kind != 4) return;
    const uint32_t x = e.h(0x1630, 0, u, 0, 0, 0);
    const uint32_t y = e.h(0x1660, 0, u, 0, 0, 0);
    static const int32_t dx[5] = {0, 1, 1, -1, -1}, dy[5] = {0, 1, -1, 1, -1};
    for (uint32_t code = 0x75; ; code = 0x52) {
        for (int k = 0; k < 5; ++k) e.h(0xAFF10, 0, 0, u, 0, 0, code, x + (uint32_t)dx[k], y + (uint32_t)dy[k], 0, 1);
        if (code == 0x52) break;
    }
    e.h(0x88A70, 0, 0, 0, 0x99A, 0, u, 0, 0, 0);
}
}
