// 1.13c D2Client+0x1FC40 -- thiscall, ECX = this, one stack word (ret 4). A non-zero argument logs through 0xC550 (this, 0xB8, 1, 1, the
// MonUnique.cpp source name, line 0x157), then 0xC838 (ECX = this, EDX = 0xB8, stack 4) and hands that result to 0xCDC0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
void f1fc40(GE& e, uint32_t self, uint32_t arg)
{
    if (!arg) return;
    e.h(0xC550, 0, 0, 0, 0, 0, self, 0xB8, 1, 1, 0x6fb86d3c, 0x157);
    const uint32_t r = e.h(0xC838, 0, self, 0xB8, 0, 0, 4);
    e.h(0xCDC0, 0, 0, 0, 0, 0, r);
}
}
