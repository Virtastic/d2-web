// 1.13c D2Client+0x96690, written from the disassembly: weapon-swap body-slot click for hand slot 4 (body in d2client_slot45.h).
// ECX = p1 (unit), EDX = p2 (inventory key), one stack argument A (ret 4).
#include "d2client_slot45.h"
// 1.13c D2Client+0x96690
extern "C" uint32_t d2c_96690(uint32_t p1, uint32_t p2, uint32_t a, uint32_t gA, uint32_t gB, const cl::Env& e) {
    return d2c_slot45::run(p1, p2, a, gA, gB, e, 4);
}
