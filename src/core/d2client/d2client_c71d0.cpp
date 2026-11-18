// 1.13c D2Client+0xC71D0 -- skill do: a thrown or shot item. Takes the unit's weapon (slot query 1), marks the unit
// once its frame counter has reached 9, refuses a weapon that is neither stackable-free nor passes the 0x7D check,
// and launches the weapon's missile: the eight-word routine for item type 0x26, the plain launch otherwise.
#include "cl_skdo.h"
extern "C" uint32_t d2c_c71d0(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) {
    const uint32_t it = e.H(0xc6ee, u, 1);
    if (!it) return 0;
    e.H(0xc76c, it);
    if ((int32_t)(e.R(u + 0x38) & 0xffffff00u) >= 0x900) e.W(u + 0xc4, e.R(u + 0xc4) | 0x40);
    if (!e.H(0xc76c, it) && e.H(0xc454, it, 0x7d, 0)) return 0;
    const int32_t m = (int32_t)e.H(0xc736, it);
    if (m <= 0 || m >= (int32_t)e.R(e.dt + 0xb6c)) return 0;
    if (e.H(0xc1cc, it, 0x26)) return clsk::launch8(e, u, m, sk, lvl) != 0;
    clsk::launch3(e, u, m, sk, lvl);
    return 1;
}
