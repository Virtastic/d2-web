// 1.13c D2Game+0x4EBE0: ecx = unit, edx = arg, one ignored stack word (ret 4).
// Calls 0xE0460(eax = unit, stack 1), #10945 (unit, arg, 0) at thunk 0xA3CA, aborts through #10265/#10024/_exit(-1) on a null unit
// (never taken in a vector: it ends the process), then 0xEFC60 with esi = unit and edi = unit->+0x80.
#include "d2game_seam4e.h"
// 1.13c D2Game+0x4EBE0
extern "C" void d2game_4ebe0(uint32_t unit, uint32_t arg, const D2Seam* e)
{
    e->call(0xE0460, unit, 0, 0, 0, 0, 1);
    e->call(0xA3CA, 0, 0, 0, 0, 0, unit, arg, 0);
    if (unit == 0) {
        const uint32_t msg = e->call(0xA2DA, 0, 0, 0, 0, 0, 0xa2a);
        e->call(0xA2C2, 0, 0, 0, 0, 0, 0x6fd19ff9, msg);
        e->call(0x22A3C, 0, 0, 0, 0, 0, 0xffffffffu);
        return;
    }
    e->call(0xEFC60, 0, 0, 0, 0, unit);   // edi = d2rd32(unit + 0x80), not logged by the recorder
}
