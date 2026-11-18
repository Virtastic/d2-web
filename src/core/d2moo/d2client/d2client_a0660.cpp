// EBX = player unit, stack arg 0 = state word (ret 4).
// When the state word is 0 and both hand items (inventory slots 4 and 5) pass the 0x2D stat test, the active
// weapon-switch record is re-read and pushed back to the unit.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xA0660
extern "C" void d2c_a0660(uint32_t unit, uint32_t state, const cl::Env& e) {
    if (e.R(state) != 0) return;
    const uint32_t inv = e.R(unit + 0x60);
    auto hand = [&](uint32_t slot) {
        uint32_t x = e.H(0xc262, inv, slot);                       // D2Common 11139 (inventory item at body slot)
        if (x && (!e.H(0xc724, x) || e.H(0xc220, x) == 0x26)) x = 0;  // 11006 / 11088
        return x;
    };
    const uint32_t a = hand(4), b = hand(5);
    if (!e.H(0xc1cc, a, 0x2d) || !e.H(0xc1cc, b, 0x2d)) return;    // 10744
    uint32_t s = e.H(0xc4ea, 0xffffffffu, 0, 0, 0, 0, 0, unit, 0);  // 10708: ecx = unit, edx = 0, stack -1
    if (!e.H(0xc79c, s)) {                                          // 10631
        e.W(state, 5);
        e.H(0xc74e, s, 5);                                          // 11031
        s = e.H(0xc4ea, 0xffffffffu, 0, 0, 0, 0, 0, unit, 5);
        e.H(0xc778, unit, s);                                       // 11103
        return;
    }
    e.H(0xc74e, s, 0);
    e.H(0xc778, unit, s);
}
