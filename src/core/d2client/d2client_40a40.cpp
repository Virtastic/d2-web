// 1.13c D2Client+0x40A40 -- skill do: runs the level routine (ECX = unit, stack word = level); on success the unit is
// marked as having acted.
#include "cl_skdo.h"
extern "C" uint32_t d2c_40a40(uint32_t u, int32_t, uint32_t lvl, const cl::Env& e) {
    if (!e.H(0x409b0, lvl, 0, 0, 0, 0, 0, u)) return 0;
    clsk::acted(e, u);
    return 1;
}
