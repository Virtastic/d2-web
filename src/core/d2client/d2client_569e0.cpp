// 1.13c D2Client+0x569E0 -- skill do: a relocation. Takes the target position from the unit's skill-target record,
// clears the record's two position fields, finds the room for the position and moves the unit there.
#include "cl_skdo.h"
extern "C" uint32_t d2c_569e0(uint32_t u, int32_t, uint32_t, const cl::Env& e) {
    const uint32_t p = e.H(0xc310, u);
    if (!p) return 0;
    const uint32_t y = e.H(0xcb6e, p);
    const uint32_t z = e.H(0xcb9e, p);
    if (!y || !z) return 0;
    e.H(0xcb7a, p, 0);
    e.H(0xcb8c, p, 0);
    const uint32_t r = e.H(0xc1f0, u);
    const uint32_t rm = e.H(0x1400, 0, 0, 0, 0, 0, r, 0, 0, y);
    if (!rm) return 0;
    e.H(0xcb98, rm, y, z, 1, 0x100);
    return 1;
}
