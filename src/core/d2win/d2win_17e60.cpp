// 1.13c D2Win+0x17e60 -- stdcall(msg): when the module flag at image offset 0x214C8 is set and the Storm-side probe 0x7714 answers nonzero,
// toggles through 0x7708 / 0x76fc / 0x770e / 0x7702 and marks msg[6] = 1, msg[7] = 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2win_214c8;
namespace d2w {
void f17e60(GE& e, uint32_t* msg)
{
    if (!g_d2win_214c8) return;
    if (!e.h(0x7714, 0, 0, 0, 0, 0)) return;
    uint32_t ecx;
    if (e.h(0x7708, 0, 0, 0, 0, 0)) { e.h(0x76fc, 0, 0, 0, 0, 0); ecx = 0; }
    else { e.h(0x770e, 0, 1, 0, 0, 0); ecx = 1; }
    e.h(0x7702, 0, ecx, 0, 0, 0);
    msg[6] = 1;
    msg[7] = 0;
}
}
