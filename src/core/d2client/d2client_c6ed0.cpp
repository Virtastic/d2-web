// 1.13c D2Client+0xC6ED0 -- skill do: acts on the mode word of the unit's target record. Bit 0x100 first asks the
// two-word routine (a refusal without bit 2 ends with 0); bit 2 sends the unit's two path coordinates on and clears
// the word; bit 0x80 runs the two follow-up routines; otherwise a non-player unit has the word set to 0x1000.
#include "cl_skdo.h"
extern "C" uint32_t d2c_c6ed0(uint32_t u, int32_t, uint32_t, const cl::Env& e) {
    const uint32_t t = e.H(0xc838, 0, 0, 0, 0, 0, 0, u);
    if (!t) return 0;
    const uint32_t x = e.H(0xc628, t);
    if ((x & 0x100) && !e.H(0xc6aa0, t, 1, 0, 0, 0, u) && !(x & 2)) return 0;
    if (x & 2) {
        const uint32_t a = e.PD(u, 0xc2ec, 0x10);
        const uint32_t b = e.PD(u, 0xc3be, 0xc);
        e.H(0xc6060, a, 0, 0, 0, 0, 0, 0, 0, b, u);
        e.H(0xc60a, t, 0);
        return 1;
    }
    if (x & 0x80) {
        e.H(0xc5f20, t);
        e.H(0xc65b0, u, 0, 0, 0, 0, t);
        return 1;
    }
    if (!(u && e.R(u) == 1)) e.H(0xc60a, t, 0x1000);
    return 1;
}
