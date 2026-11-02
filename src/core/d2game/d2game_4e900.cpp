// 1.13c D2Game+0x4E900: eax = unit, one stack argument (ret 4). Hands 0x4E390 (eax = flag, ecx = argument, 0, 1) the flag from the 0x15F stat step on a unit carrying state 0x61.
// Calls: #10494 (thunk 0xA68E), #10871 (0xA5D4), #10680 (0xA712).
#include "d2game_seam4e.h"
extern "C" void d2game_4e900(uint32_t unit, uint32_t arg, const D2Seam* e)
{
    uint32_t ebx = 1;
    if (e->call(0xA68E, 0, 0, 0, 0, 0, unit, 0x61)) {
        const uint32_t st = e->call(0xA5D4, 0, 0, 0, 0, 0, unit, 0x61);
        if (st) {
            e->call(0xA712, 0, 0, 0, 0, 0, st, 0x15e, 0);
            ebx = e->call(0xA712, 0, 0, 0, 0, 0, st, 0x15f, 0);
        }
    }
    e->call(0x4E390, ebx, arg, 0, 0, 0, 0, 1);
}
